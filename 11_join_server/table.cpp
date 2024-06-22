#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <boost/assert.hpp>
#include <stdexcept>

#include "table.hpp"

namespace NDatabase {
    TRowData TRowData::Empty(TRowId rowId, std::size_t capacity) {
        TRowData out(capacity);
        out.Append(int(rowId));

        for (std::size_t i = 1; i < capacity; i++) {
            out.Append(TNull{});
        }

        return out;
    };

    TRowData TRowData::MergeWith(const TRowData& other) const {
        TRowData out(Values_.size() + other.Values_.size() - 1);

        for (std::size_t i = 0; i < Values_.size(); i++) {
            out.Append(Values_[i]);
        }

        // omit first item as it contains row id
        for (std::size_t i = 1; i < other.Values_.size(); i++) {
            out.Append(other.Values_[i]);
        }

        return out;
    }

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

    std::size_t TRowData::Dump(std::string& buffer) const {
        const auto bufSize = buffer.size();
        std::size_t offset = 0;
        std::size_t item = 0;

        auto visitor = [&](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;

            std::string added;

            if constexpr (std::is_same_v<T, int>) {
                added = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                added = arg;
            } else if constexpr (std::is_same_v<T, TNull>) {
                added = "";
            } else {
                throw std::invalid_argument("unknown type");
            }

            if (offset + added.size() > buffer.size()) {
                throw std::runtime_error("insufficient buffer length");
            }

            for (std::size_t j = 0; j < added.size(); j++) {
                buffer.at(offset) = added.at(j);
                offset++;
            }

            if (item != Values_.size() - 1) {
                buffer.at(offset) = ',';
            } else {
                buffer.at(offset) = '\n';
            }

            offset++;
            item++;
        };

        for (const auto& value : Values_) {
            std::visit(visitor, value);
        }

        return offset;
    }

    std::optional<TRowData> TRow::GetVersion(TTxId txId) {
        std::shared_lock lock{Mutex_};

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
        return lastValid->RowData_;
    }

    void TRow::AddVesion(TTxId txId, std::optional<TRowData> rowData) {
        auto t = rowData.has_value() ? rowData->Get<int>(0) : -1;

        std::unique_lock lock{Mutex_};

        if (Versions_.empty()) {
            Versions_.emplace_back(TRowVersion{txId, std::move(rowData)});
            return;
        }

        BOOST_ASSERT(Versions_.back().TxId_ < txId);
        Versions_.emplace_back(TRowVersion{txId, std::move(rowData)});
    }

    void TTable::InsertRow(TTxId txId, const TRowData& rowData) {
        // first column always contains primary key - it's the part of our contract
        TRowId rowId = rowData.Get<int>(0);

        {
            // hot path: row exists, so we need only add one more version
            std::shared_lock lock{Mutex_};
            auto it = Rows_.find(rowId);
            if (it != Rows_.end()) {
                it->second.AddVesion(txId, rowData);
                return;
            }
        }

        {
            // no such row, need to create new one, so hold an exclusive lock to modify index
            std::unique_lock lock{Mutex_};

            // now we're sure, create new one
            Rows_[rowId].AddVesion(txId, rowData);
        }
    }

    std::optional<TRowData> TTable::GetRow(TTxId txId, TRowId rowId) {
        std::shared_lock lock{Mutex_};

        auto it = Rows_.find(rowId);
        if (it == Rows_.end()) {
            return std::nullopt;
        }

        return it->second.GetVersion(txId);
    }

    void TTable::Iterate(TTxId txId, TRowHandler handler) {
        // not possible to add new rows, other transactions can only read and modify existing rows
        std::shared_lock lock{Mutex_};

        for (auto& [k, v] : Rows_) {
            handler(k, v.GetVersion(txId));
        }
    }

    void TTable::Truncate(TTxId txId) {
        // Table truncation is implemented via appending empty version to each row;
        // there's no need to lock parallel transactions.
        std::shared_lock lock{Mutex_};

        for (auto& [k, v] : Rows_) {
            v.AddVesion(txId, std::nullopt);
        }
    }

    TTable::TPtr MakeTable() {
        return std::make_unique<TTable>();
    }
} //namespace NDatabase
