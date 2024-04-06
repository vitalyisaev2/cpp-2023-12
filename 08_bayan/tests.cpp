
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <gtest/gtest.h>
#include <optional>

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

    NBayan::TBlockChecksumStorage blockChecksumStorage;

    std::optional<NBayan::TBlockChecksumStorage::TResult> actual;
    NBayan::TBlockChecksumStorage::TResult expected;
    actual = blockChecksumStorage.RegisterBlock(f1, 0, 111);
    ASSERT_EQ(actual, std::nullopt);

    actual = blockChecksumStorage.RegisterBlock(f2, 0, 111);
    expected = {.BlockID = 1, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f1}};
    ASSERT_EQ(*actual, expected);
}
