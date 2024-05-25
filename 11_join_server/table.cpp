#include "table.hpp"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <mutex>
#include <optional>
#include <shared_mutex>

namespace NDatabase {
    bool TRowData::operator==(const TRowData& other) const {
        if (Values_.size() != other.Values_.size()) {
            return false;
        }

        for (std::size_t i = 0; i < Values_.size(); i++) {
            if (Values_[i] != other.Values_[i]) {
                return false;
            }
        }

        return true;
    }

    std::optional<TRowData> TRow::GetVersion(TTxId txId) {
        std::shared_lock lock{Mutex};

        // iterate through version list to find a version the most recent
        // for a given transaction id
        auto lastValid = Versions_.end();
        auto current = Versions_.begin();
        for (; current != Versions_.end(); current++) {
            if (current->TxId_ <= txId) {
                lastValid = current;
            } else {
                break;
            }
        }

        if (lastValid == Versions_.end()) {
            return std::nullopt;
        }

        // TODO: instead of copying smart pointers
        return *lastValid->RowData_;
    }

    void TRow::AddVesion(TTxId txId, TRowData rowData) {
        std::unique_lock lock{Mutex};

        if (Versions_.empty()) {
            Versions_.emplace_back(TRowVersion{txId, std::move(rowData)});
            return;
        }

        BOOST_ASSERT(Versions_.back().TxId_ < txId);
        Versions_.emplace_back(TRowVersion{txId, std::move(rowData)});
    }
} //namespace NDatabase
