#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>
#include <map>
#include <memory>
#include <variant>
#include <vector>

namespace NDatabase {
    using TTxId = std::size_t;

    // Currently our database can only keep ints and strings.
    using TValue = std::variant<int, std::string>;

    class TRowData {
    public:
        TRowData() = delete;

        explicit TRowData(const std::vector<TValue> values)
            : Values_(std::move(values)) {
        }

        explicit TRowData(std::size_t capacity) {
            Values_.reserve(capacity);
        }

        template <typename T>
        void Append(T value) {
            Values_.emplace_back(std::move(value));
        }

        template <typename T>
        T Get(std::size_t position) const {
            if (std::holds_alternative<T>(Values_[position])) {
                return std::get<T>(Values_[position]);
            }

            std::stringstream ss;
            ss << "Expected " << typeid(T).name() << "for a value on a position " << position
               << ", got something different";
            throw std::runtime_error(ss.str());
        }

        bool operator==(const TRowData& other) const;

    private:
        std::vector<TValue> Values_;
    };

    using TRowHandler = std::function<void(const TRowData&)>;

    struct TRowVersion {
        // Every row version gets a transaction id assigned
        // at the moment of row version creation.
        TTxId TxId_;

        // The row data itself.
        // If it's empty, that means that the row was removed by the transcation.
        std::optional<TRowData> RowData_;

        bool operator==(const TRowVersion& other) const {
            return std::tie(TxId_, RowData_) == std::tie(other.TxId_, other.RowData_);
        }
    };

    class TRow {
    public:
        std::optional<TRowData> GetVersion(TTxId txId);
        void AddVesion(TTxId txId, TRowData rowData);

    private:
        // We keep several versions of the same row for the sake of MVCC
        // (multiversion concurrency control)
        std::list<TRowVersion> Versions_;

        // Mutex synchronizes access to the list of row versions.
        std::shared_mutex Mutex;
    };

    class TTable {
    public:
        using TPtr = std::unique_ptr<TTable>;

        void InsertRow(TTxId txId, const TRowData& rowData);
        void Iterate(TTxId txId, TRowHandler handler);
        TRowData GetRow(TTxId txId);
        void Truncate(TTxId txId);

    private:
        using TRowId = std::size_t;

        // This is the simpliest kind of index. It maps primary key into the row.
        std::map<TRowId, TRow> Rows_;

        // Mutex synchronizes access to the rows;
        std::shared_mutex Mutex_;
    };
} //namespace NDatabase
