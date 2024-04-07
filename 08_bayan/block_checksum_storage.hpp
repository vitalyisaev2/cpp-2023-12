#pragma once

#include <cstdint>
#include <unordered_set>
#include <memory>
#include <optional>

#include <boost/filesystem/path.hpp>

namespace NBayan {
    class TBlockChecksumStorage {
    public:
        struct TPathHasher {
            std::size_t operator()(const boost::filesystem::path& p) const noexcept {
                return std::hash<std::string>()(p.string());
            }
        };

        using TFilenameSet = std::unordered_set<boost::filesystem::path, TPathHasher>;

    private:
        struct TNode {
            using TPtr = std::shared_ptr<TNode>;

            TNode(std::optional<boost::filesystem::path> filename, std::optional<std::size_t> blockID,
                  uint32_t blockChecksum)
                : BlockID(blockID)
                , BlockChecksum(blockChecksum) {
                if (filename) {
                    IsTrailingBlockForFilenames.emplace(std::move(*filename));
                }
            };

            std::optional<std::size_t> BlockID;
            uint32_t BlockChecksum;
            TFilenameSet IsTrailingBlockForFilenames;
            std::unordered_map<uint32_t, TPtr> Children;
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
                                             uint32_t blockChecksum);

    private:
        TNode::TPtr Root = std::make_shared<TNode>(std::nullopt, std::nullopt, 0);
        std::unordered_map<boost::filesystem::path, TNode::TPtr, TPathHasher> Filenames;
    };
} //namespace NBayan
