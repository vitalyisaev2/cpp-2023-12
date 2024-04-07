#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include <boost/filesystem.hpp>

#include "block_checksum_storage.hpp"
#include "file_block_checksum_computer.hpp"
#include "utils.hpp"

namespace NBayan {
    class TFileCrawler {
    public:
        TFileCrawler(TFileBlockChecksumComputer fileBlockChecksumComputer,
                     const TBlockChecksumStorage::TPtr& blockChecksumStorage)
            : FileBlockChecksumComputer(std::move(fileBlockChecksumComputer))
            , BlockChecksumStorage(blockChecksumStorage) {
        }

    private:
        struct TTask {
            std::size_t BlockId;
            boost::filesystem::path Filename;
        };

    public:
        void Run(std::vector<boost::filesystem::path> roots);

    private:
        void HandleNextTask();

        std::queue<TTask> Tasks;
        TFileBlockChecksumComputer FileBlockChecksumComputer;
        TBlockChecksumStorage::TPtr BlockChecksumStorage;

        // FileReadState tracks the state of the particular file reading.
        // If it's false, that means that the file has been read to the end,
        // no other blocks left.
        std::unordered_map<boost::filesystem::path, bool, TPathHasher> FileReadState;
    };
} //namespace NBayan
