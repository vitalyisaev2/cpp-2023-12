#pragma once

#include <cstddef>
#include <string>

#include <boost/multiprecision/cpp_int.hpp>

namespace NBayan {
    enum class EChecksumType { CRC32 = 1, MD5 = 2 };

    EChecksumType EChecksumTypeFromString(const std::string& src);
    std::string EChecksumTypeToString(EChecksumType src);

    class TChecksumComputer {
    public:
        // NOTE: we can extend this type up to 1024bit integer if necessary
        using TChecksum = boost::multiprecision::uint128_t;

        TChecksumComputer(EChecksumType checksumType)
            : ChecksumType(checksumType){};

        TChecksum Compute(const char* start, std::size_t size, std::size_t zeroBytesTail = 0) const;

    private:
        TChecksum ComputeCRC32(const char* start, std::size_t size, std::size_t zeroBytesTail = 0) const;

        const EChecksumType ChecksumType;
    };
} //namespace NBayan
