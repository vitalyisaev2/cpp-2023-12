#include <optional>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <gtest/gtest.h>

#include "file_checker.hpp"
#include "block_checksum_storage.hpp"

TEST(FileChecker, File) {
    // create temporary file with some data
    auto filepath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::ofstream ofs(filepath);
    ofs << "12345678";
    ofs.close();

    // block size 8 == equal to file size
    NBayan::TFileChecker fileChecker8(8, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    auto result = fileChecker8.ComputeFileBlockHash(filepath, 0);

    ASSERT_EQ(result.Value, 2598427311);
    ASSERT_EQ(result.HasNext, false);

    // block size 4 == divisor of the file size
    NBayan::TFileChecker fileChecker4(4, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker4.ComputeFileBlockHash(filepath, 0);
    ASSERT_EQ(result.Value, 2615402659);
    ASSERT_EQ(result.HasNext, true);
    result = fileChecker4.ComputeFileBlockHash(filepath, 1);
    ASSERT_EQ(result.Value, 2119325191);
    ASSERT_EQ(result.HasNext, false);

    // block size 16 == much greater than the file size
    NBayan::TFileChecker fileChecker16(16, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker16.ComputeFileBlockHash(filepath, 0);
    ASSERT_EQ(result.Value, 2483966577);
    ASSERT_EQ(result.HasNext, false);

    // cleanup file
    boost::filesystem::remove(filepath);
}

TEST(BlockChecksumStorage, SimpleTree) {
    boost::filesystem::path f1("/tmp/f1.txt");
    boost::filesystem::path f2("/tmp/f2.txt");
    boost::filesystem::path f3("/tmp/f3.txt");

    NBayan::TBlockChecksumStorage blockChecksumStorage;

    std::optional<NBayan::TBlockChecksumStorage::TResult> actual;
    NBayan::TBlockChecksumStorage::TResult expected;

    // add first file - nothing to check next
    actual = blockChecksumStorage.RegisterBlock(f1, 0, 111);
    ASSERT_EQ(actual, std::nullopt);

    // add second file with the similair block - need to next block of both files 
    actual = blockChecksumStorage.RegisterBlock(f2, 0, 111);
    expected = {.BlockID = 1, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f1, f2}};
    ASSERT_EQ(*actual, expected);

    // in the next block files have diverged
    actual = blockChecksumStorage.RegisterBlock(f1, 1, 2221);
    ASSERT_EQ(actual, std::nullopt);

    actual = blockChecksumStorage.RegisterBlock(f2, 1, 2222);
    ASSERT_EQ(actual, std::nullopt);

    // add third file - storage asks for the next block of only this file
    actual = blockChecksumStorage.RegisterBlock(f3, 0, 111);
    expected = {.BlockID = 1, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f3}};
    ASSERT_EQ(*actual, expected);

    // add next block for third file - it appears to be equal to the block of the first file,
    // so we have to these two files further 
    actual = blockChecksumStorage.RegisterBlock(f3, 1, 2221);
    expected = {.BlockID = 2, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f1, f3}};
    ASSERT_EQ(*actual, expected);
}
