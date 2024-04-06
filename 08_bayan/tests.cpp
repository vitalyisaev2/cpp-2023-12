
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <gtest/gtest.h>

#include "file_checker.hpp"

TEST(FileChecker, File) {
    // create temporary file with some data
    auto filepath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::ofstream ofs(filepath);
    ofs << "12345678";
    ofs.close();

    // block size 8 == equal to file size
    NBayan::TFileChecker fileChecker8(8, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    auto result = fileChecker8.ComputeFileBlockHash(filepath, 0);

    ASSERT_EQ(result, 2598427311);

    // block size 4 == divisor of the file size 
    NBayan::TFileChecker fileChecker4(4, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker4.ComputeFileBlockHash(filepath, 0);
    ASSERT_EQ(result, 2615402659);
    result = fileChecker4.ComputeFileBlockHash(filepath, 1);
    ASSERT_EQ(result, 2119325191);

    // block size 16 == much greater than the file size
    NBayan::TFileChecker fileChecker16(16, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    result = fileChecker16.ComputeFileBlockHash(filepath, 0);
    ASSERT_EQ(result, 2483966577);

    // cleanup file
    boost::filesystem::remove(filepath);

}
