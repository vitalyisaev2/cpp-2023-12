#include <optional>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <gtest/gtest.h>

#include "checksum_computer.hpp"
#include "file_block_checksum_computer.hpp"
#include "block_checksum_storage.hpp"
#include "file_crawler.hpp"

TEST(FileChecker, File) {
    // create temporary file with some data
    auto filepath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::ofstream ofs(filepath);
    ofs << "12345678";
    ofs.close();

    // block size 8 == equal to file size
    NBayan::TFileBlockChecksumComputer fileChecker8(8, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    auto result = fileChecker8.Compute(filepath, 0);

    ASSERT_EQ(result.Value, 2598427311);
    ASSERT_EQ(result.HasNext, false);

    // block size 4 == divisor of the file size
    NBayan::TFileBlockChecksumComputer fileChecker4(4, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker4.Compute(filepath, 0);
    ASSERT_EQ(result.Value, 2615402659);
    ASSERT_EQ(result.HasNext, true);
    result = fileChecker4.Compute(filepath, 1);
    ASSERT_EQ(result.Value, 2119325191);
    ASSERT_EQ(result.HasNext, false);

    // block size 16 == much greater than the file size
    NBayan::TFileBlockChecksumComputer fileChecker16(16, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker16.Compute(filepath, 0);
    ASSERT_EQ(result.Value, 2483966577);
    ASSERT_EQ(result.HasNext, false);

    // cleanup file
    boost::filesystem::remove(filepath);
}

TEST(BlockChecksumStorage, SimpleTree) {
    // These are just pathes, not real files
    boost::filesystem::path f1("/tmp/f1.txt");
    boost::filesystem::path f2("/tmp/f2.txt");
    boost::filesystem::path f3("/tmp/f3.txt");

    NBayan::TBlockChecksumStorage blockChecksumStorage;

    std::optional<NBayan::TBlockChecksumStorage::TRegisterResult> actual;
    NBayan::TBlockChecksumStorage::TRegisterResult expected;

    // add first file - nothing to check next
    actual = blockChecksumStorage.Register(f1, 0, 111);
    ASSERT_EQ(actual, std::nullopt);

    // add second file with the similair block - need to next block of both files
    actual = blockChecksumStorage.Register(f2, 0, 111);
    expected = {.BlockID = 1, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f1, f2}};
    ASSERT_EQ(*actual, expected);

    // in the next block files have diverged
    actual = blockChecksumStorage.Register(f1, 1, 2221);
    ASSERT_EQ(actual, std::nullopt);

    actual = blockChecksumStorage.Register(f2, 1, 2222);
    ASSERT_EQ(actual, std::nullopt);

    // add third file - storage asks for the next block of only this file
    actual = blockChecksumStorage.Register(f3, 0, 111);
    expected = {.BlockID = 1, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f3}};
    ASSERT_EQ(*actual, expected);

    // add next block for third file - it appears to be equal to the block of the first file,
    // so we have to these two files further
    actual = blockChecksumStorage.Register(f3, 1, 2221);
    expected = {.BlockID = 2, .Filenames = NBayan::TBlockChecksumStorage::TFilenameSet{f1, f3}};
    ASSERT_EQ(*actual, expected);
}

void CreateFileWithContent(const boost::filesystem::path& filename, const std::string& data) {
    boost::filesystem::ofstream ofs(filename);
    ofs << data;
    ofs.close();
}

TEST(FileCrawler, PlainDirSomeDuplicates) {
    auto tempdir = boost::filesystem::temp_directory_path() / boost::filesystem::path("bayan_tests");
    boost::filesystem::create_directory(tempdir);

    auto f1 = tempdir / boost::filesystem::path("f1.txt");
    auto f2 = tempdir / boost::filesystem::path("f2.txt");
    auto f3 = tempdir / boost::filesystem::path("f3.txt");

    CreateFileWithContent(f1, "12345678");
    CreateFileWithContent(f2, "12345678");
    CreateFileWithContent(f3, "12341234");

    NBayan::TFileCrawler fileCrawler(
        NBayan::TFileBlockChecksumComputer(4, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32)),
        NBayan::TBlockChecksumStorage());

    fileCrawler.Run({tempdir});

    boost::filesystem::remove_all(tempdir);
}
