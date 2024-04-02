#include <stdexcept>

#include <boost/assert/source_location.hpp>
#include <boost/throw_exception.hpp>
#include <boost/crc.hpp>
#include <vector>

#include "checksum_computer.hpp"

namespace NBayan {

    uint32_t TChecksumComputer::Compute(const char* start, std::size_t size, std::size_t zeroBytesTail) const {
        switch (ChecksumType) {
            case EChecksumType::CRC32:
                return ComputeCRC32(start, size, zeroBytesTail);
            default:
                boost::throw_exception(std::invalid_argument("unexpected checksum type"), BOOST_CURRENT_LOCATION);
        }
    }

    uint32_t TChecksumComputer::ComputeCRC32(const char* start, std::size_t size, std::size_t zeroBytesTail) const {
        boost::crc_32_type result;
        result.process_bytes(start, size);

        if (zeroBytesTail) {
            std::vector<char> tail(zeroBytesTail);
            result.process_bytes(tail.data(), zeroBytesTail);
        }

        return result.checksum();
    }

} //namespace NBayan
