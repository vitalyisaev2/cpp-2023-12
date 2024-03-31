#pragma once

#include <cstdint>

#include <string>

namespace NBayan {

    struct TFile {
        std::string Name;
    };

    class TFileChecker {
    public:
        TFileChecker(std::size_t blockSize) noexcept
            : BlockSize(blockSize){};

        uint32_t ComputeFileBlockHash(const TFile& file, std::size_t blockId) const;

    private:
        std::size_t BlockSize;
    };

} //namespace NBayan
