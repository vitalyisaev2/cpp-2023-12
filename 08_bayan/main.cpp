#include <iostream>

#include <boost/exception/diagnostic_information.hpp>

#include "checksum_computer.hpp"
#include "file_block_checksum_computer.hpp"

int main() {
    try {
        NBayan::TFileBlockChecksumComputer fileChecker(1024, NBayan::TChecksumComputer(NBayan::EChecksumType::CRC32));
    } catch (std::exception const& x) {
        std::cerr << boost::diagnostic_information(x) << std::endl;
    }
}
