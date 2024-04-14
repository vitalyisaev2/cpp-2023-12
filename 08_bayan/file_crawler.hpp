#pragma once

#include <boost/filesystem/operations.hpp>
#include <queue>
#include <unordered_map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/log/trivial.hpp>

#include "block_checksum_storage.hpp"
#include "file_block_checksum_computer.hpp"
#include "utils.hpp"

namespace NBayan {
    class TFileCrawler {
    public:
        TFileCrawler(TFileBlockChecksumComputer fileBlockChecksumComputer,
                     const TBlockChecksumStorage::TPtr& blockChecksumStorage,
                     std::vector<boost::filesystem::path> included, bool recursive, std::size_t minFileSize)
            : FileBlockChecksumComputer(std::move(fileBlockChecksumComputer))
            , BlockChecksumStorage(blockChecksumStorage)
            , Included(included)
            , Recursive(recursive)
            , MinFileSize(minFileSize) {
        }

        // Run is a top-level function that begins filesystem scanning
        void Run();

    private:
        struct TTask {
            std::size_t BlockId;
            boost::filesystem::path Filename;
        };

        // Tasks for file block reading a stored within a queue
        std::queue<TTask> Tasks;

        // HandleNextTask extracts next task from the queue and checks it
        void HandleNextTask();

        using dir_iterator = boost::filesystem::directory_iterator;
        using recursive_dir_iterator = boost::filesystem::recursive_directory_iterator;

        // IterateDirectory finds and filters files to check
        template <class It>
        void IterateDirectory(const boost::filesystem::path& dirPath) {
            for (const auto& path : It(dirPath)) {
                if (!boost::filesystem::is_regular_file(path)) {
                    continue;
                }

                const auto fileSize = boost::filesystem::file_size(path);

                if (fileSize < MinFileSize) {
                    BOOST_LOG_TRIVIAL(debug) << "Omitting file as too small: " << path;
                    continue;
                }

                if (fileSize == 0) {
                    BOOST_LOG_TRIVIAL(debug) << "Omitting empty file: " << path;
                    BlockChecksumStorage->RegisterEmpty(std::move(path));
                    continue;
                }

                BOOST_LOG_TRIVIAL(debug) << "Discovered non-empty file: " << path;
                Tasks.push(TTask{.BlockId = 0, .Filename = std::move(path)});
            }
        }

        TFileBlockChecksumComputer FileBlockChecksumComputer;
        TBlockChecksumStorage::TPtr BlockChecksumStorage;
        std::vector<boost::filesystem::path> Included;
        bool Recursive;
        std::size_t MinFileSize;

        // FileReadState tracks the progress of the particular file reading.
        // It contains the number of block that should be read the next time.
        // If it's nullopt, that means that the file has been read to the end,
        // and no other blocks left.
        std::unordered_map<boost::filesystem::path, std::optional<std::size_t>, TPathHasher> FileReadState;
    };
} //namespace NBayan
