#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <cstddef>
#include <stdexcept>
#include <vector>

#include <boost/assert/source_location.hpp>
#include <boost/crc.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/throw_exception.hpp>
#include <openssl/md5.h>

#include "checksum_computer.hpp"

namespace NBayan {
    EChecksumType EChecksumTypeFromString(const std::string& src) {
        if (src == "CRC32")
            return EChecksumType::CRC32;
        if (src == "MD5")
            return EChecksumType::MD5;

        boost::throw_exception(std::invalid_argument("unexpected string to convert into checksum type"),
                               BOOST_CURRENT_LOCATION);
    }

    std::string EChecksumTypeToString(EChecksumType src) {
        switch (src) {
            case NBayan::EChecksumType::CRC32:
                return "CRC32";
            case NBayan::EChecksumType::MD5:
                return "MD5";
        }

        boost::throw_exception(std::invalid_argument("unexpected checksum type value"), BOOST_CURRENT_LOCATION);
    }

    TChecksumComputer::TChecksum TChecksumComputer::Compute(const char* start, std::size_t size,
                                                            std::size_t zeroBytesTail) const {
        switch (ChecksumType) {
            case EChecksumType::CRC32:
                return ComputeCRC32(start, size, zeroBytesTail);
            case EChecksumType::MD5:
                return ComputeMD5(start, size, zeroBytesTail);
            default:
                boost::throw_exception(std::invalid_argument("unexpected checksum type"), BOOST_CURRENT_LOCATION);
        }
    }

    TChecksumComputer::TChecksum TChecksumComputer::ComputeCRC32(const char* start, std::size_t size,
                                                                 std::size_t zeroBytesTail) const {
        boost::crc_32_type result;
        result.process_bytes(start, size);

        if (zeroBytesTail) {
            std::vector<char> tail(zeroBytesTail);
            result.process_bytes(tail.data(), zeroBytesTail);
        }

        return TChecksum(result.checksum());
    }

    TChecksumComputer::TChecksum TChecksumComputer::ComputeMD5(const char* start, std::size_t size,
                                                               std::size_t zeroBytesTail) const {
        MD5_CTX c;
        MD5_Init(&c);
        MD5_Update(&c, start, size);

        if (zeroBytesTail) {
            // allocate tail buffer that contains only zeroes
            std::vector<char> tail(zeroBytesTail);
            std::fill(tail.begin(), tail.end(), 0);
            MD5_Update(&c, tail.data(), zeroBytesTail);
        }

        unsigned char dst[MD5_DIGEST_LENGTH];
        MD5_Final(dst, &c);

        std::stringstream ss;
        ss << "0x";
        for (std::size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(dst[i]);
        }

        return TChecksum(ss.str());
    }

} //namespace NBayan
