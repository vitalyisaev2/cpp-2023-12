#include <memory>
#include <shared_mutex>
#include <sstream>

#include "database.hpp"
#include "table.hpp"
#include "thread_pool.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {

    TResultQueue::TPtr MakeResultQueue() {
        return MakeThreadSafeQueue<TResult>();
    };

    TDatabase::TPtr TDatabase::Create() {
        return std::shared_ptr<TDatabase>(new TDatabase());
    };

    TOperationStatus TDatabase::Insert(const std::string& tableName, TRowData&& rowData) {
        return ThreadPool_
            ->Enqueue([self = shared_from_this(), tableName = tableName,
                       rowData = std::move(rowData)](std::size_t) -> TOperationStatus {
                {
                    // check if table with the requested name exists
                    std::shared_lock lock{self->Mutex_};
                    auto it = self->Tables_.find(tableName);
                    if (it != self->Tables_.end()) {
                        auto txId = ++self->TxCounter_;
                        it->second->InsertRow(txId, std::move(rowData));
                        return TOperationStatus::Success();
                    }
                }

                {
                    // re-check and create table if necessary
                    std::unique_lock lock{self->Mutex_};
                    auto txId = ++self->TxCounter_;

                    auto it = self->Tables_.find(tableName);
                    if (it != self->Tables_.end()) {
                        it->second->InsertRow(txId, std::move(rowData));
                        return TOperationStatus::Success();
                    }

                    auto table = MakeTable();
                    table->InsertRow(txId, std::move(rowData));

                    self->Tables_.emplace(tableName, std::move(table));
                    return TOperationStatus::Success();
                }
            })
            .get();
    }

    TOperationStatus TDatabase::Select(const std::string& tableName, TResultQueue::TPtr queue) {
        return ThreadPool_
            ->Enqueue([self = shared_from_this(), tableName = tableName,
                       queue = std::move(queue)](std::size_t) -> TOperationStatus {
                std::shared_lock lock{self->Mutex_};
                auto it = self->Tables_.find(tableName);
                if (it == self->Tables_.cend()) {
                    std::stringstream ss;
                    ss << "Table with name '" << tableName << "' does not exist";
                    return TOperationStatus{.Succeeded = false, .Message = std::move(ss.str())};
                }

                auto txId = ++self->TxCounter_;
                it->second->Iterate(
                    txId, [&queue](TRowId rowId, std::optional<TRowData> rowData) { queue->Push(std::move(rowData)); });

                queue->Push(TEndOfTable{});
                return TOperationStatus::Success();
            })
            .get();
    }

    TOperationStatus TDatabase::Truncate(const std::string& tableName) {
        return ThreadPool_
            ->Enqueue([self = shared_from_this(), tableName = tableName](std::size_t) -> TOperationStatus {
                std::shared_lock lock{self->Mutex_};
                auto it = self->Tables_.find(tableName);
                if (it == self->Tables_.cend()) {
                    std::stringstream ss;
                    ss << "Table with name '" << tableName << "' does not exist";
                    return TOperationStatus{.Succeeded = false, .Message = std::move(ss.str())};
                }

                auto txId = ++self->TxCounter_;
                it->second->Truncate(txId);
                return TOperationStatus::Success();
            })
            .get();
    }

} //namespace NDatabase
