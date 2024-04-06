#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/assert/source_location.hpp>
#include <boost/throw_exception.hpp>
#include <iterator>

#include "file_checker.hpp"

namespace NBayan {
    TFileChecker::TResult TFileChecker::ComputeFileBlockHash(const boost::filesystem::path& filepath,
                                                             std::size_t blockId) const {
        const boost::interprocess::mode_t mode = boost::interprocess::read_only;
        boost::interprocess::file_mapping fm(filepath.c_str(), mode);
        boost::interprocess::mapped_region region(fm, mode, 0, 0);

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
            return TResult{.Value = value, .HasNext = true};
        } else {
            auto value = ChecksumComputer.Compute(blockStart, region.get_size() - blockStartShift,
                                                  blockEndShift - region.get_size());
            return TResult{.Value = value, .HasNext = false};
        }
    }
} //namespace NBayan
