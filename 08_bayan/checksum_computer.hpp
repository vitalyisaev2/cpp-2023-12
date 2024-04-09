#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>

namespace NBayan {
    enum class EChecksumType { CRC32 = 1 };

    std::istream& operator>>(std::istream& in, EChecksumType& verboseLevel);

    class TChecksumComputer {
    public:
        TChecksumComputer(EChecksumType checksumType)
            : ChecksumType(checksumType){};

        uint32_t Compute(const char* start, std::size_t size, std::size_t zeroBytesTail = 0) const;

    private:
        uint32_t ComputeCRC32(const char* start, std::size_t size, std::size_t zeroBytesTail = 0) const;

        const EChecksumType ChecksumType;
    };
} //namespace NBayan
