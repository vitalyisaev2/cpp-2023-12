#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>

#include "block_checksum_storage.hpp"
#include "file_block_checksum_computer.hpp"
#include "utils.hpp"

namespace NBayan {
    class TFileCrawler {
    public:
        TFileCrawler(TFileBlockChecksumComputer fileBlockChecksumComputer,
                     TBlockChecksumStorage::TPtr blockChecksumStorage,
                     const std::vector<boost::filesystem::path>& included, 
                     const std::vector<boost::filesystem::path>& excluded,
                     std::optional<boost::regex> fileMask,
                     bool recursive, 
                     std::size_t minFileSize)
            : FileBlockChecksumComputer(std::move(fileBlockChecksumComputer))
            , BlockChecksumStorage(std::move(blockChecksumStorage))
            , Included(TransformPathesToAbsolute(included))
            , Excluded(TransformPathesToAbsolute(excluded))
            , FileMask(std::move(fileMask))
            , Recursive(recursive)
            , MinFileSize(minFileSize) {
        }

        // Run is a top-level function that begins filesystem scanning
        void Run();

    private:
        static std::vector<boost::filesystem::path>
        TransformPathesToAbsolute(const std::vector<boost::filesystem::path>& src);

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

        // IterateDirectory finds and filters files
        template <class It>
        void IterateDirectory(const boost::filesystem::path& dirPath) {
            for (auto& path : It(dirPath)) {
                HandleDirectoryEntry(path);
            }
        }

        void HandleDirectoryEntry(const boost::filesystem::directory_entry& path);

        TFileBlockChecksumComputer FileBlockChecksumComputer;
        TBlockChecksumStorage::TPtr BlockChecksumStorage;
        std::vector<boost::filesystem::path> Included;
        std::vector<boost::filesystem::path> Excluded;
        std::optional<boost::regex> FileMask;
        bool Recursive;
        std::size_t MinFileSize;

        // FileReadState tracks the progress of the particular file reading.
        // It contains the number of block that should be read the next time.
        // If it's nullopt, that means that the file has been read to the end,
        // and no other blocks left.
        std::unordered_map<boost::filesystem::path, std::optional<std::size_t>, TPathHasher> FileReadState;
    };
} //namespace NBayan
