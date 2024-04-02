#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "file_checker.hpp"

namespace NBayan {
    uint32_t TFileChecker::ComputeFileBlockHash(const TFile& file, std::size_t blockId) const {
        const boost::interprocess::mode_t mode = boost::interprocess::read_only;
        boost::interprocess::file_mapping fm(file.Name.c_str(), mode);
        boost::interprocess::mapped_region region(fm, mode, 0, 0);

        const char* blockStart = static_cast<const char*>(region.get_address());
        // shift to the first byte of requested block
        blockStart += blockId * BlockSize;

        const char* blockEnd = blockStart + BlockSize;
        const char* regionEnd = static_cast<const char*>(region.get_address()) + region.get_size();
        if (blockEnd >= regionEnd) {
            blockEnd = regionEnd;
            // TODO: need to fill up to block size
        }

        return 0;
    }
} //namespace NBayan
