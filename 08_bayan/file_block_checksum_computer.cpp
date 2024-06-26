#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/assert/source_location.hpp>
#include <boost/throw_exception.hpp>
#include <boost/log/trivial.hpp>

#include "file_block_checksum_computer.hpp"

namespace NBayan {
    TFileBlockChecksumComputer::TComputeResult
    TFileBlockChecksumComputer::Compute(const boost::filesystem::path& filename, std::size_t blockId) const {
        // BOOST_LOG_TRIVIAL(debug) << "Trying to compute hash: filename=" << filename << ", blockId=" << blockId;

        const boost::interprocess::mode_t mode = boost::interprocess::read_only;

        boost::interprocess::file_mapping fm(filename.c_str(), mode);
        boost::interprocess::mapped_region region(fm, mode, 0, 0);

        // empty file
        if (region.get_size() == 0) {
            auto value = ChecksumComputer.Compute(nullptr, 0, 1024);
            return TComputeResult{.Value = value, .NextBlockID = std::nullopt};
        }

        // invalid request: trying to read non-existing block
        const auto blockStartShift = blockId * BlockSize;
        if (blockStartShift >= region.get_size()) {
            boost::throw_exception(std::invalid_argument("block start shift is too large"), BOOST_CURRENT_LOCATION);
        }

        // shift to the first byte of requested block
        const char* blockStart = static_cast<const char*>(region.get_address());
        std::advance(blockStart, blockStartShift);

        const auto blockEndShift = blockStartShift + BlockSize;
        if (blockEndShift < region.get_size()) {
            auto value = ChecksumComputer.Compute(blockStart, BlockSize, 0);
            return TComputeResult{.Value = value, .NextBlockID = blockId + 1};
        } else {
            auto value = ChecksumComputer.Compute(blockStart, region.get_size() - blockStartShift,
                                                  blockEndShift - region.get_size());
            return TComputeResult{.Value = value, .NextBlockID = std::nullopt};
        }
    }
} //namespace NBayan
