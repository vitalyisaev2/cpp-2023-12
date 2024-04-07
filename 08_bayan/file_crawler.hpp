#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include "block_checksum_storage.hpp"
#include "file_block_checksum_computer.hpp"
#include "utils.hpp"

namespace NBayan {
    class TFileCrawler {
    public:
        TFileCrawler(TFileBlockChecksumComputer fileBlockChecksumComputer, TBlockChecksumStorage blockChecksumStorage)
            : FileBlockChecksumComputer(std::move(fileBlockChecksumComputer))
            , BlockChecksumStorage(std::move(blockChecksumStorage)) {
        }

    private:
        struct TTask {
            std::size_t BlockId;
            boost::filesystem::path Filename;
        };

    public:
        struct TResult {
            using TGroup = std::vector<boost::filesystem::path>;

            std::vector<TGroup> Groups;
        };

        TResult Run(std::vector<boost::filesystem::path> roots) {
            for (const auto& root : roots) {
                BOOST_VERIFY_MSG(boost::filesystem::is_directory(root),
                                 "block ID for the file that is registered for the first time must be equal to 0");

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

            return TResult{};
        }

    private:
        void HandleNextTask() {
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
            auto registerResult = BlockChecksumStorage.Register(task.Filename, task.BlockId, computeResult.Value);
            if (registerResult) {
                for (const auto& path : registerResult->Filenames) {
                    Tasks.push(TTask{.BlockId = registerResult->BlockID, .Filename = path});
                }
            }
        }

        std::queue<TTask> Tasks;
        TFileBlockChecksumComputer FileBlockChecksumComputer;
        TBlockChecksumStorage BlockChecksumStorage;

        // FileReadState tracks the state of the particular file reading.
        // If it's false, that means that the file has been read to the end,
        // no other blocks left.
        std::unordered_map<boost::filesystem::path, bool, TPathHasher> FileReadState;
    };
} //namespace NBayan
