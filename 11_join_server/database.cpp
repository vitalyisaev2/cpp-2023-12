#include <memory>
#include <shared_mutex>
#include <sstream>

#include "database.hpp"
#include "table.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {

    TResultQueue::TPtr MakeResultQueue() {
        return MakeThreadSafeQueue<TResult>();
    };

    TDatabase::TPtr TDatabase::Create() {
        return std::shared_ptr<TDatabase>(new TDatabase());
    };

    TResultQueue::TPtr TDatabase::Insert(const std::string& tableName, TRowData&& rowData) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, rowData = std::move(rowData),
                              queue = queue](std::size_t) -> void {
            {
                // check if table with the requested name exists
                std::shared_lock lock{self->Mutex_};
                auto it = self->Tables_.find(tableName);
                if (it != self->Tables_.end()) {
                    auto txId = ++self->TxCounter_;
                    it->second->InsertRow(txId, std::move(rowData));
                    queue->Push(TOperationStatus::Success());
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
                    queue->Push(TOperationStatus::Success());
                    return;
                }

                auto table = MakeTable();
                table->InsertRow(txId, std::move(rowData));

                self->Tables_.emplace(tableName, std::move(table));
                queue->Push(TOperationStatus::Success());
                return;
            }
        });

        return queue;
    }

    TResultQueue::TPtr TDatabase::Select(const std::string& tableName) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, queue = queue](std::size_t) -> void {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                queue->Push(TOperationStatus::Error(ss.str()));
                return;
            }

            auto txId = ++self->TxCounter_;
            it->second->Iterate(
                txId, [&queue](TRowId rowId, std::optional<TRowData> rowData) { queue->Push(std::move(rowData)); });

            queue->Push(TOperationStatus::Success());
            return;
        });

        return queue;
    }

    TResultQueue::TPtr TDatabase::Truncate(const std::string& tableName) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, queue = queue](std::size_t) -> void {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                queue->Push(TOperationStatus::Error(ss.str()));
                return;
            }

            auto txId = ++self->TxCounter_;
            it->second->Truncate(txId);

            queue->Push(TOperationStatus::Success());
            return;
        });

        return queue;
    }

} //namespace NDatabase
