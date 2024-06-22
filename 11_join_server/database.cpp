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
                return Intersection(arg.TableName1_, arg.TableName2_);
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
                ss << "ERR: table with name '" << tableName << "' does not exist";
                queue->Push(TStatus::Error(ss.str()));
                return;
            }

            auto txId = ++self->TxCounter_;
            it->second->Iterate(txId, [&queue](TRowId rowId, std::optional<TRowData> rowData) {
                if (rowData) {
                    queue->Push(std::move(rowData));
                }
            });

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
                ss << "ERR: table with name '" << tableName << "' does not exist";
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

#define mustFindTable(tableIt, tableName)                                  \
    auto tableIt = self->Tables_.find(tableName);                          \
    if (tableIt == self->Tables_.cend()) {                                 \
        std::stringstream ss;                                              \
        ss << "ERR: table with name '" << tableName << "' does not exist"; \
        queue->Push(TStatus::Error(ss.str()));                             \
        return;                                                            \
    }

    TDatabase::TResultQueue::TPtr TDatabase::Intersection(const std::string& tableName1,
                                                          const std::string& tableName2) {
        auto queue = MakeResultQueue();

        ThreadPool_->Enqueue([self = shared_from_this(), tableName1 = tableName1, tableName2 = tableName2,
                              queue = queue](std::size_t) -> void {
            std::shared_lock lock{self->Mutex_};

            // obtain tables
            mustFindTable(tableIt1, tableName1);
            mustFindTable(tableIt2, tableName2);

            auto txId = ++self->TxCounter_;
            auto& table2 = tableIt2->second;

            tableIt1->second->Iterate(txId, [&queue, txId, &table2](TRowId rowId, std::optional<TRowData> rowData1) {
                if (rowData1) {
                    auto rowData2 = table2->GetRow(txId, rowId);
                    if (rowData2) {
                        queue->Push(rowData1->MergeWith(*rowData2));
                    }
                }
            });

            queue->Push(TStatus::Success());
            return;
        });

        return queue;
    }

} //namespace NDatabase
