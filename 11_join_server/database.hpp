#pragma once

#include <atomic>
#include <shared_mutex>

#include "table.hpp"
#include "thread_pool.hpp"
#include "status.hpp"

namespace NDatabase {
    // TDatabase manages the dataset and replies to the requests of a server.
    class TDatabase: public std::enable_shared_from_this<TDatabase> {
    public:
        // TResult represents an elementary item of a data stream, produced by an asynchronous database operation.
        // It can contain either a table row, or a status of a operation.
        using TResult = std::variant<std::optional<TRowData>, TStatus>;

        // TResultQueue is used by TCP server to obtain the response data from the database
        using TResultQueue = TThreadSafeQueue<TResult>;

    public:
        using TPtr = std::shared_ptr<TDatabase>;
        static TPtr Create() {
            return std::shared_ptr<TDatabase>(new TDatabase());
        };

        TDatabase(const TDatabase&) = delete;
        TDatabase(TDatabase&&) = delete;
        TDatabase& operator=(const TDatabase&) = delete;
        TDatabase& operator=(TDatabase&&) = delete;

        TResultQueue::TPtr Insert(const std::string& tableName, TRowData&& rowData);
        TResultQueue::TPtr Select(const std::string& tableName);
        TResultQueue::TPtr Truncate(const std::string& tableName);
        TResultQueue::TPtr Intersection(const std::string& tableName1, const std::string& tableName2);
        TResultQueue::TPtr SymmetricDifference(const std::string& tableName1, const std::string& tableName2);

    private:
        static TResultQueue::TPtr MakeResultQueue() {
            return MakeThreadSafeQueue<TDatabase::TResult>();
        };

    private:
        TDatabase()
            : ThreadPool_(TThreadPool::Create(4)){};

        std::unordered_map<std::string, TTable::TPtr> Tables_; // set of tables containing the data
        std::shared_mutex Mutex_;                              // synchronizes access to tables map

        std::atomic<TTxId> TxCounter_; // provides unique ids for transactions

        TThreadPool::TPtr ThreadPool_; // handles tasks in parallel threads
    };
} //namespace NDatabase
