#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

#include "file_crawler.hpp"

namespace NBayan {
    std::vector<boost::filesystem::path>
    TFileCrawler::TransformPathesToAbsolute(const std::vector<boost::filesystem::path>& src) {
        std::vector<boost::filesystem::path> dst;
        dst.reserve(src.size());

        for (const auto& item : src) {
            BOOST_VERIFY_MSG(boost::filesystem::is_directory(item), "path is not a dir");
            dst.emplace_back(boost::filesystem::absolute(item));
        }

        return dst;
    }

    void TFileCrawler::Run() {
        for (const auto& root : Included) {
            BOOST_LOG_TRIVIAL(debug) << "Traversing root: " << root;

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

    void TFileCrawler::HandleDirectoryEntry(const boost::filesystem::directory_entry& path) {
        if (!boost::filesystem::is_regular_file(path)) {
            return;
        }

        // omit files belonging to excluded directory
        for (const auto& excl : Excluded) {
            auto pathStr = path.path().string();
            auto exclStr = excl.string();
            auto res = std::mismatch(exclStr.begin(), exclStr.end(), pathStr.begin());
            if (res.first == exclStr.end()) {
                return;
            }
        }

        // omit files that doesn't match file mask
        if (FileMask) {
            boost::smatch what;
            if (!boost::regex_match(path.path().string(), what, *FileMask)) {
                return;
            }
        }

        const auto fileSize = boost::filesystem::file_size(path);

        if (fileSize < MinFileSize) {
            BOOST_LOG_TRIVIAL(debug) << "Omitting file as too small: " << path;
            return;
        }

        if (fileSize == 0) {
            BOOST_LOG_TRIVIAL(debug) << "Omitting empty file: " << path;
            BlockChecksumStorage->RegisterEmpty(std::move(path));
            return;
        }

        BOOST_LOG_TRIVIAL(debug) << "Discovered non-empty file: " << path;
        Tasks.push(TTask{.BlockId = 0, .Filename = std::move(path)});
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
