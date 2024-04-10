#include <boost/assert.hpp>
#include <vector>

#include "block_checksum_storage.hpp"
#include "checksum_computer.hpp"

namespace NBayan {
    std::optional<TBlockChecksumStorage::TRegisterResult>
    TBlockChecksumStorage::Register(const boost::filesystem::path& filename, std::size_t blockID,
                                    TChecksumComputer::TChecksum blockChecksum) {
        auto filenamesIter = Filenames.find(filename);

        // If this file wasn't registered before, save the first block and exit.
        TNode::TPtr parent = nullptr;
        if (filenamesIter == Filenames.end()) {
            BOOST_VERIFY_MSG(blockID == 0,
                             "block ID for the file that is registered for the first time must be equal to 0");
            parent = Root;
        } else {
            parent = filenamesIter->second;
            BOOST_VERIFY_MSG(parent->BlockID = blockID - 1, "block IDs must be contiguous");
        }

        // Find the latest (to this moment) block corresponding to the file.
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

        return std::make_optional<TRegisterResult>({blockID + 1, child->IsTrailingBlockForFilenames});
    }

    TBlockChecksumStorage::TGetDuplicatesResult TBlockChecksumStorage::GetDuplicates() const {
        // Invert the Filenames -> Node mapping
        std::unordered_map<TNode::TPtr, TGetDuplicatesResult::TGroup> nodesToFilesMap;
        for (const auto [path, node] : Filenames) {
            auto nodesIt = nodesToFilesMap.find(node);
            if (nodesIt == nodesToFilesMap.end()) {
                nodesToFilesMap[node] = TGetDuplicatesResult::TGroup{path};
            } else {
                nodesIt->second.emplace(path);
            }
        };

        // Move map content to the resulting container
        TBlockChecksumStorage::TGetDuplicatesResult result;
        for (auto& kv: nodesToFilesMap) {
            if (kv.second.size() > 1) {
                // duplicate found
                result.Groups.emplace_back(std::move(kv.second));
            }
        }

        return result;
    }
} //namespace NBayan
