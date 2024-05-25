#pragma once

#include <atomic>

#include "table.hpp"

namespace NDatabase {
    class TDatabase {
    public:
        void Insert(const std::string& table, const TRowData& rowData);
        void Truncate(const std::string& table);
        void Intersection(const std::string& table1, const std::string& table2) const;
        void SymmetricDifference(const std::string& table1, const std::string& table2) const;

    private:
        std::map<std::string, TTable::TPtr> Tables_;
        std::atomic<TTxId> TxCounter_;
    };
} //namespace NDatabase
