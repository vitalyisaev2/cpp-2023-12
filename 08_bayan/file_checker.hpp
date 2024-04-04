#pragma once

#include <cstdint>

#include <boost/filesystem.hpp>

#include "checksum_computer.hpp"

namespace NBayan {

    class TFileChecker {
    public:
        TFileChecker(std::size_t blockSize, TChecksumComputer checksumComputer) noexcept
            : BlockSize(blockSize)
            , ChecksumComputer(std::move(checksumComputer)){};

        uint32_t ComputeFileBlockHash(const boost::filesystem::path& filepath, std::size_t blockId) const;

    private:
        const std::size_t BlockSize;
        const TChecksumComputer ChecksumComputer;
    };

} //namespace NBayan
