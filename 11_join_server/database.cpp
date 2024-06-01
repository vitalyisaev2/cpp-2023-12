#include <shared_mutex>
#include <sstream>

#include "database.hpp"
#include "table.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {

    TResultQueue::TPtr MakeResultQueue() {
        return MakeThreadSafeQueue<TResult>();
    };

    void TDatabase::Insert(const std::string& tableName, TRowData&& rowData) {
        ThreadPool_->Enqueue(
            [self = shared_from_this(), tableName = tableName, rowData = std::move(rowData)](std::size_t) {
                {
                    // check if table with the requested name exists
                    std::shared_lock lock{self->Mutex_};
                    auto it = self->Tables_.find(tableName);
                    if (it != self->Tables_.end()) {
                        auto txId = ++self->TxCounter_;
                        it->second->InsertRow(txId, std::move(rowData));
                        return;
                    }
                }

                {
                    // re-check and create table if necessary
                    std::unique_lock lock{self->Mutex_};
                    auto txId = ++self->TxCounter_;

                    auto it = self->Tables_.find(tableName);
                    if (it != self->Tables_.end()) {
                        it->second->InsertRow(txId, std::move(rowData));
                        return;
                    }

                    auto table = MakeTable();
                    table->InsertRow(txId, std::move(rowData));

                    self->Tables_.emplace(tableName, std::move(table));
                }
            });
    }

    void TDatabase::Select(const std::string& tableName, TResultQueue::TPtr queue) {
        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, queue = std::move(queue)](std::size_t) {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                throw ss.str();
            }

            auto txId = ++self->TxCounter_;
            it->second->Iterate(
                txId, [&queue](TRowId rowId, std::optional<TRowData> rowData) { queue->Push(std::move(rowData)); });
        });
    }

    void TDatabase::Truncate(const std::string& tableName) {
        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName](std::size_t) {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                throw ss.str();
            }

            auto txId = ++self->TxCounter_;
            it->second->Truncate(txId);
        });
    }

} //namespace NDatabase
