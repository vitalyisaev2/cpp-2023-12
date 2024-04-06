#pragma once

#include <cstdint>
#include <unordered_set>
#include <memory>
#include <optional>

#include <boost/assert.hpp>
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

            TNode(boost::filesystem::path filename, std::size_t blockID, uint32_t blockChecksum)
                : BlockID(blockID)
                , BlockChecksum(blockChecksum) {
                IsTrailingBlockForFilenames.emplace(std::move(filename));
            };

            std::size_t BlockID;
            uint32_t BlockChecksum;
            TFilenameSet IsTrailingBlockForFilenames;
            std::unordered_map<uint32_t, TPtr> Children;
        };

    public:
        // TResult describes the filenames which blocks need to be computed and registered next;
        struct TResult {
            std::size_t BlockID;
            std::unordered_set<boost::filesystem::path, TPathHasher> Filenames;

            bool operator==(const TResult& other) const  {
                return (BlockID == other.BlockID) && (Filenames == other.Filenames);
            }
        };

        std::optional<TResult> RegisterBlock(const boost::filesystem::path& filename, std::size_t blockID,
                                             uint32_t blockChecksum) {
            auto filenamesIter = Filenames.find(filename);

            // If this file wasn't registered before, save the first block and exit.
            if (filenamesIter == Filenames.end()) {
                BOOST_VERIFY_MSG(blockID == 0, "block ID of previously unregistered file must be equal to 0");

                auto rootsIter =
                    Roots.emplace(blockChecksum, std::make_shared<TNode>(filename, blockID, blockChecksum));

                Filenames[filename] = rootsIter.first->second;

                return std::nullopt;
            }

            // Find the latest (to this moment) block corresponding to the file.
            auto& parent = filenamesIter->second;
            BOOST_VERIFY_MSG(parent->BlockID = blockID - 1, "block IDs must be contiguous");

            auto childrenIter = parent->Children.find(blockChecksum);

            // No block with this checksum found, save this block and exit.
            if (childrenIter == parent->Children.end()) {
                auto child = std::make_shared<TNode>(filename, blockID, blockChecksum);
                parent->Children[blockChecksum] = child;
                Filenames[filename] = std::move(child);
                parent->IsTrailingBlockForFilenames.erase(filename);

                return std::nullopt;
            }

            // Child with similair block id found;
            // we need to check next blocks for some files.
            auto& child = childrenIter->second;
            child->IsTrailingBlockForFilenames.emplace(filename);
            Filenames[filename] = child;

            return std::make_optional<TResult>({blockID + 1, child->IsTrailingBlockForFilenames});
        };

    private:
        std::unordered_map<uint32_t, TNode::TPtr> Roots;
        std::unordered_map<boost::filesystem::path, TNode::TPtr, TPathHasher> Filenames;
    };
} //namespace NBayan
