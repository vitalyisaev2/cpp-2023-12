#include <boost/filesystem/directory.hpp>

#include "file_crawler.hpp"

namespace NBayan {
    void TFileCrawler::Run() {
        for (const auto& root : Included) {
            BOOST_LOG_TRIVIAL(debug) << "Traversing root: " << root;
            BOOST_VERIFY_MSG(boost::filesystem::is_directory(root), "path is not a dir");

            // Walk the directory tree and collect the filenames.
            if (Recursive) {
                IterateDirectory<recursive_dir_iterator>(root);
            } else {
                IterateDirectory<dir_iterator>(root);
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

        auto fileReadStateIt = FileReadState.find(task.Filename);
        if (fileReadStateIt != FileReadState.end()) {

            // ignore read request if file has been already read until the end
            if (!fileReadStateIt->second) {
                // std::cout << "IGNORE 1" << std::endl;
                return;
            }

            // outdated read
            if (task.BlockId < fileReadStateIt->second.value()) {
                // std::cout << "IGNORE 2: " << task.BlockId << " " << fileReadStateIt->second.value() << std::endl;
                return;
            }
        }

        // compute checksum of the requested block
        auto computeResult = FileBlockChecksumComputer.Compute(task.Filename, task.BlockId);
        // preserve information about the next block that should be read
        FileReadState[task.Filename] = computeResult.NextBlockID;

        // register checksum in the storage
        auto registerResult = BlockChecksumStorage->Register(task.Filename, task.BlockId, computeResult.Value);
        if (registerResult) {
            for (const auto& path : registerResult->Filenames) {
                // std::cout << "RESULT: " << path << " " << registerResult->BlockID << std::endl;
                Tasks.push(TTask{.BlockId = registerResult->BlockID, .Filename = path});
            }
        }
    }
} //namespace NBayan
