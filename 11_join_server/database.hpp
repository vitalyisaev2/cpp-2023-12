#pragma once

#include <atomic>
#include <shared_mutex>

#include "table.hpp"
#include "thread_pool.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {
    // TEndOfTable is a special type marking the end of table.
    struct TEndOfTable {};

    // TResult represents an elementary item of table reading process.
    // It can contain either a table row, or a special message marking the end of table reading.
    using TResult = std::variant<std::optional<TRowData>, TEndOfTable>;

    // TResultQueue is used by TCP server to obtain the response data from the database
    using TResultQueue = TThreadSafeQueue<TResult>;
    TResultQueue::TPtr MakeResultQueue();

    struct TOperationStatus {
        bool Succeeded;
        std::optional<std::string> Message;

        static TOperationStatus Success() {
            return TOperationStatus{.Succeeded = true, .Message = std::nullopt};
        };
    };

    // TDatabase manages the dataset and replies to the requests of a server.
    class TDatabase: public std::enable_shared_from_this<TDatabase> {
    public:
        using TPtr = std::shared_ptr<TDatabase>;
        static TPtr Create();

        TDatabase(const TDatabase&) = delete;
        TDatabase(TDatabase&&) = delete;
        TDatabase& operator=(const TDatabase&) = delete;
        TDatabase& operator=(TDatabase&&) = delete;

        TOperationStatus Insert(const std::string& tableName, TRowData&& rowData);
        TOperationStatus Select(const std::string& tableName, TResultQueue::TPtr queue);
        TOperationStatus Truncate(const std::string& tableName);
        TOperationStatus Intersection(const std::string& tableName1, const std::string& tableName2,
                                      TResultQueue::TPtr queue);
        TOperationStatus SymmetricDifference(const std::string& tableName1, const std::string& tableName2,
                                             TResultQueue::TPtr queue);

    private:
        TDatabase()
            : ThreadPool_(MakeThreadPool<TOperationStatus>(4)){};

        std::unordered_map<std::string, TTable::TPtr> Tables_; // set of tables containing the data
        std::shared_mutex Mutex_;                              // synchronizes access to tables map

        std::atomic<TTxId> TxCounter_; // provides unique ids for transactions

        TThreadPool<TOperationStatus>::TPtr ThreadPool_; // handles tasks in parallel threads
    };
} //namespace NDatabase
