#pragma once

#include <cstdint>
#include <string>

#include "checksum_computer.hpp"

namespace NBayan {

    struct TFile {
        std::string Name;
    };

    class TFileChecker {
    public:
        TFileChecker(std::size_t blockSize, TChecksumComputer checksumComputer) noexcept
            : BlockSize(blockSize)
            , ChecksumComputer(std::move(checksumComputer)){};

        uint32_t ComputeFileBlockHash(const TFile& file, std::size_t blockId) const;

    private:
        const std::size_t BlockSize;
        const TChecksumComputer ChecksumComputer;
    };

} //namespace NBayan
