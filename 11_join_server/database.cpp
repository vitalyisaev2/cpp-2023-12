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
        auto visitor = [&](auto& arg) -> TDatabase::TResultQueue::TPtr {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, TCmdInsert>) {
                return Insert(arg.TableName_, std::move(arg.RowData_));
            } else if constexpr (std::is_same_v<T, TCmdSelect>) {
                return Select(arg.TableName_);
            } else if constexpr (std::is_same_v<T, TCmdTruncate>) {
                return Truncate(arg.TableName_);
            } else if constexpr (std::is_same_v<T, TCmdDifference>) {
                throw std::invalid_argument("unknown type");
                // return Select(arg.TableName_);
            } else if constexpr (std::is_same_v<T, TCmdIntersect>) {
                throw std::invalid_argument("unknown type");
                // return Select(arg.TableName_);
            } else {
                throw std::invalid_argument("unknown type");
            }
        };

        return std::visit(visitor, command);
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
