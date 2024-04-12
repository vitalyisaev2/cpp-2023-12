#pragma once

#include <boost/filesystem.hpp>

#include "checksum_computer.hpp"

namespace NBayan {

    class TFileBlockChecksumComputer {
    public:
        TFileBlockChecksumComputer(std::size_t blockSize, TChecksumComputer checksumComputer) noexcept
            : BlockSize(blockSize)
            , ChecksumComputer(std::move(checksumComputer)){};

        struct TComputeResult {
            TChecksumComputer::TChecksum Value;
            bool HasNext;
        };

        TComputeResult Compute(const boost::filesystem::path& filepath, std::size_t blockId) const;
    private:
        const std::size_t BlockSize;
        const TChecksumComputer ChecksumComputer;
    };

} //namespace NBayan
