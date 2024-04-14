#pragma once

#include <boost/filesystem.hpp>
#include <cstddef>

#include "checksum_computer.hpp"

namespace NBayan {

    class TFileBlockChecksumComputer {
    public:
        TFileBlockChecksumComputer(std::size_t blockSize, TChecksumComputer checksumComputer) noexcept
            : BlockSize(blockSize)
            , ChecksumComputer(std::move(checksumComputer)){};

        struct TComputeResult {
            TChecksumComputer::TChecksum Value;
            std::optional<std::size_t> NextBlockID;
        };

        TComputeResult Compute(const boost::filesystem::path& filepath, std::size_t blockId) const;

    private:
        const std::size_t BlockSize;
        const TChecksumComputer ChecksumComputer;
    };

} //namespace NBayan
