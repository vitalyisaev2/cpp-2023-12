
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <gtest/gtest.h>

#include "file_checker.hpp"

TEST(FileChecker, Simple) {
    NBayan::TFileChecker fileChecker(8, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));

    // create temporary file with some data
    auto filepath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::ofstream ofs(filepath);
    ofs << "12345678";

    auto result = fileChecker.ComputeFileBlockHash(filepath, 0);

    // cleanup
    boost::filesystem::remove(filepath);

    ASSERT_EQ(result, 2598427311);
}
