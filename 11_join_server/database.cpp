#include <memory>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>

#include "database.hpp"
#include "commands.hpp"
#include "table.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {
    TDatabase::TResultQueue::TPtr TDatabase::HandleCommand(TCmd&& command) {
        // Handle various commands
        if (std::holds_alternative<TCmdInsert>(command)) {
            auto cast = std::get<TCmdInsert>(command);
            return Insert(cast.TableName_, std::move(cast.RowData_));
        } else if (std::holds_alternative<TCmdSelect>(command)) {
            auto cast = std::get<TCmdSelect>(command);
            return Select(cast.TableName_);
        } else {
            throw std::invalid_argument("unknown command");
        }
    }

    TDatabase::TResultQueue::TPtr TDatabase::Insert(const std::string& tableName, TRowData&& rowData) {
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
                    queue->Push(TStatus::Success());
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
                    queue->Push(TStatus::Success());
                    return;
                }

                auto table = MakeTable();
                table->InsertRow(txId, std::move(rowData));

                self->Tables_.emplace(tableName, std::move(table));
                queue->Push(TStatus::Success());
                return;
            }
        });

        return queue;
    }

    TDatabase::TResultQueue::TPtr TDatabase::Select(const std::string& tableName) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, queue = queue](std::size_t) -> void {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                queue->Push(TStatus::Error(ss.str()));
                return;
            }

            auto txId = ++self->TxCounter_;
            it->second->Iterate(
                txId, [&queue](TRowId rowId, std::optional<TRowData> rowData) { queue->Push(std::move(rowData)); });

            queue->Push(TStatus::Success());
            return;
        });

        return queue;
    }

    TDatabase::TResultQueue::TPtr TDatabase::Truncate(const std::string& tableName) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName = tableName, queue = queue](std::size_t) -> void {
            std::shared_lock lock{self->Mutex_};
            auto it = self->Tables_.find(tableName);
            if (it == self->Tables_.cend()) {
                std::stringstream ss;
                ss << "Table with name '" << tableName << "' does not exist";
                queue->Push(TStatus::Error(ss.str()));
                return;
            }

            auto txId = ++self->TxCounter_;
            it->second->Truncate(txId);

            queue->Push(TStatus::Success());
            return;
        });

        return queue;
    }

} //namespace NDatabase
