#pragma once

#include <atomic>
#include <shared_mutex>

#include "table.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {
    using TResultQueue = TThreadSafeQueue<std::optional<TRowData>>;

    TResultQueue::TPtr MakeResultQueue();

    class TDatabase {
    public:
        void Insert(const std::string& tableName, TRowData&& rowData);
        void Select(const std::string& tableName, TResultQueue::TPtr queue);
        void Truncate(const std::string& tableName);
        void Intersection(const std::string& tableName1, const std::string& tableName2, TResultQueue::TPtr queue);
        void SymmetricDifference(const std::string& tableName1, const std::string& tableName2,
                                 TResultQueue::TPtr queue);

    private:
        std::unordered_map<std::string, TTable::TPtr> Tables_;
        std::shared_mutex Mutex_; // synchronizes access to tables maps
        std::atomic<TTxId> TxCounter_;
    };
} //namespace NDatabase
