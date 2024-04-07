#include <boost/log/trivial.hpp>

#include "file_crawler.hpp"

namespace NBayan {
    void TFileCrawler::Run(std::vector<boost::filesystem::path> roots) {
        for (const auto& root : roots) {
            BOOST_LOG_TRIVIAL(debug) << "Traversing dir: " << root;
            BOOST_VERIFY_MSG(boost::filesystem::is_directory(root), "path is not a dir");

            // Walk the directory tree and collect the filenames.
            for (const auto& path : boost::filesystem::recursive_directory_iterator(root)) {
                if (boost::filesystem::is_regular_file(path)) {
                    BOOST_LOG_TRIVIAL(debug) << "Discovered file: " << path;

                    // during initialization phase we only ask to compute the first block of each file
                    Tasks.push(TTask{.BlockId = 0, .Filename = path});
                }
            }
        }

        // Read file blocks and compute checksums until all the duplicates are found.
        while (Tasks.size()) {
            HandleNextTask();
            Tasks.pop();
        }

        return;
    }

    void TFileCrawler::HandleNextTask() {
        // extract next task from the queue
        auto task = Tasks.front();

        // ignore read request if file has been already read until the end
        auto fileReadStateIt = FileReadState.find(task.Filename);
        if (fileReadStateIt != FileReadState.end()) {
            if (!fileReadStateIt->second) {
                return;
            }
        }

        // compute checksum of the requested block
        auto computeResult = FileBlockChecksumComputer.Compute(task.Filename, task.BlockId);
        FileReadState[task.Filename] = computeResult.HasNext;

        // register checksum in the storage
        auto registerResult = BlockChecksumStorage->Register(task.Filename, task.BlockId, computeResult.Value);
        if (registerResult) {
            for (const auto& path : registerResult->Filenames) {
                Tasks.push(TTask{.BlockId = registerResult->BlockID, .Filename = path});
            }
        }
    }
} //namespace NBayan
