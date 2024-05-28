#include <shared_mutex>
#include <sstream>

#include "database.hpp"
#include "table.hpp"
#include "thread_safe_queue.hpp"

namespace NDatabase {

    TResultQueue::TPtr MakeResultQueue() {
        return MakeThreadSafeQueue<std::optional<TRowData>>();
    };

    void TDatabase::Insert(const std::string& tableName, TRowData&& rowData) {
        {
            // check if table with the requested name exists
            std::shared_lock lock{Mutex_};
            auto it = Tables_.find(tableName);
            if (it != Tables_.end()) {
                it->second->InsertRow(++TxCounter_, std::move(rowData));
                return;
            }
        }

        {
            // re-check and create table if neccessary
            std::unique_lock lock{Mutex_};

            auto it = Tables_.find(tableName);
            if (it != Tables_.end()) {
                it->second->InsertRow(++TxCounter_, std::move(rowData));
                return;
            }

            auto table = MakeTable();
            table->InsertRow(++TxCounter_, std::move(rowData));

            Tables_.emplace(tableName, std::move(table));
        }
    }

    void TDatabase::Select(const std::string& tableName, TResultQueue::TPtr queue) {
        std::shared_lock lock{Mutex_};
        auto it = Tables_.find(tableName);
        if (it == Tables_.cend()) {
            std::stringstream ss;
            ss << "Table with name '" << tableName << "' does not exist";
            throw ss.str();
        }

        it->second->Iterate(
            ++TxCounter_, [&queue](TRowId rowId, std::optional<TRowData> rowData) { 
                queue->Push(std::move(rowData)); });
    }

    void TDatabase::Truncate(const std::string& tableName) {
        std::shared_lock lock{Mutex_};
        auto it = Tables_.find(tableName);
        if (it == Tables_.cend()) {
            std::stringstream ss;
            ss << "Table with name '" << tableName << "' does not exist";
            throw ss.str();
        }

        it->second->Truncate(++TxCounter_);
    }

} //namespace NDatabase
