#pragma once

#include <cstdint>
#include <set>
#include <unordered_set>
#include <memory>
#include <optional>

#include <boost/filesystem/path.hpp>

#include "checksum_computer.hpp"
#include "utils.hpp"

namespace NBayan {
    class TBlockChecksumStorage {
    public:
        using TPtr = std::shared_ptr<TBlockChecksumStorage>;
        using TFilenameSet = std::unordered_set<boost::filesystem::path, TPathHasher>;

    private:
        struct TNode {
            using TPtr = std::shared_ptr<TNode>;

            TNode(std::optional<boost::filesystem::path> filename, std::optional<std::size_t> blockID,
                  TChecksumComputer::TChecksum blockChecksum)
                : BlockID(blockID)
                , BlockChecksum(blockChecksum) {
                if (filename) {
                    IsTrailingBlockForFilenames.emplace(std::move(*filename));
                }
            };

            std::optional<std::size_t> BlockID;
            TChecksumComputer::TChecksum BlockChecksum;
            TFilenameSet IsTrailingBlockForFilenames;
            std::unordered_map<TChecksumComputer::TChecksum, TPtr> Children;
        };

    public:
        // TRegisterResult provides the list of files whose next blocks must be computed and registered
        // in the further calls.
        struct TRegisterResult {
            std::size_t BlockID;
            std::unordered_set<boost::filesystem::path, TPathHasher> Filenames;

            bool operator==(const TRegisterResult& other) const {
                return (BlockID == other.BlockID) && (Filenames == other.Filenames);
            }
        };

        // RegisterBlock saves the checksum of the block into the internal storage
        // and asks caller to provide next blocks for some files if it is necessary for
        // file deduplication.
        std::optional<TRegisterResult> Register(const boost::filesystem::path& filename, std::size_t blockID,
                                                TChecksumComputer::TChecksum blockChecksum);

        struct TGetDuplicatesResult {
            using TGroup = std::set<boost::filesystem::path>;

            std::vector<TGroup> Groups;
        };

        // GetDuplicates returns the list of files with different names, but similair content
        TGetDuplicatesResult GetDuplicates() const;

    private:
        TNode::TPtr Root = std::make_shared<TNode>(std::nullopt, std::nullopt, 0);
        std::unordered_map<boost::filesystem::path, TNode::TPtr, TPathHasher> Filenames;
    };
} //namespace NBayan
