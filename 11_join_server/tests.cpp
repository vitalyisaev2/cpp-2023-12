#include <gtest/gtest.h>
#include <optional>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <variant>
#include <vector>

#include "database.hpp"
#include "table.hpp"
#include "thread_pool.hpp"

using namespace NDatabase;

TEST(TRowData, Test) {
    TRowData rowData(2);
    rowData.Append(int(1));
    rowData.Append(std::string("qwerty12345"));

    ASSERT_EQ(rowData.Get<int>(0), int(1));
    ASSERT_EQ(rowData.Get<std::string>(1), "qwerty12345");
    ASSERT_THROW(rowData.Get<int>(1), std::runtime_error);
    ASSERT_THROW(rowData.Get<std::string>(0), std::runtime_error);
    ASSERT_THROW(rowData.Get<int>(3), std::runtime_error);
}

TEST(TRow, Test) {
    std::vector<TRowData> rowDataItems{
        TRowData(std::vector<TValue>{1, "a"}),
        TRowData(std::vector<TValue>{2, "b"}),
        TRowData(std::vector<TValue>{3, "c"}),
    };

    TRow row;
    row.AddVesion(10, rowDataItems[0]);
    row.AddVesion(20, rowDataItems[1]);
    row.AddVesion(30, rowDataItems[2]);

    ASSERT_EQ(row.GetVersion(9), std::nullopt);
    ASSERT_EQ(row.GetVersion(10), rowDataItems[0]);
    ASSERT_EQ(row.GetVersion(20), rowDataItems[1]);
    ASSERT_EQ(row.GetVersion(30), rowDataItems[2]);
    ASSERT_EQ(row.GetVersion(31), rowDataItems[2]);
}

TEST(TTable, Test) {
    TTable table;
    TRowData rowData1({1, "a"});

    // Insert several rows in some order

    table.InsertRow(2, rowData1);
    ASSERT_EQ(table.GetRow(1, 1), std::nullopt);
    ASSERT_EQ(table.GetRow(10, 1), rowData1);
    ASSERT_EQ(table.GetRow(20, 1), rowData1);
    ASSERT_EQ(table.GetRow(2, 2), std::nullopt);

    TRowData rowData2({2, "b"});
    TRowData rowData3({3, "c"});
    table.InsertRow(30, rowData3);
    table.InsertRow(31, rowData2);

    // Iterate over accepted rows - everything's fine
    std::vector<std::tuple<TRowId, std::optional<TRowData>>> accepted;
    table.Iterate(32, [&accepted](TRowId rowId, std::optional<TRowData> rowData) {
        accepted.push_back(std::make_tuple(rowId, rowData));
    });
    std::sort(accepted.begin(), accepted.end(),
              [](const auto& x1, const auto& x2) { return std::get<0>(x1) < std::get<0>(x2); });

    ASSERT_EQ(accepted.size(), 3);

    auto expected = std::make_tuple<TRowId, std::optional<TRowData>>(1, rowData1);
    ASSERT_EQ(accepted[0], expected);
    expected = std::make_tuple<TRowId, std::optional<TRowData>>(2, rowData2);
    ASSERT_EQ(accepted[1], expected);
    expected = std::make_tuple<TRowId, std::optional<TRowData>>(3, rowData3);
    ASSERT_EQ(accepted[2], expected);

    // Now we truncate the table
    table.Truncate(40);

    // Every transaction occured after the truncation sees just empty rows
    accepted.clear();
    table.Iterate(41, [&accepted](TRowId rowId, std::optional<TRowData> rowData) {
        accepted.push_back(std::make_tuple(rowId, rowData));
    });
    std::sort(accepted.begin(), accepted.end(),
              [](const auto& x1, const auto& x2) { return std::get<0>(x1) < std::get<0>(x2); });

    expected = std::make_tuple<TRowId, std::optional<TRowData>>(1, std::nullopt);
    ASSERT_EQ(accepted[0], expected);
    expected = std::make_tuple<TRowId, std::optional<TRowData>>(2, std::nullopt);
    ASSERT_EQ(accepted[1], expected);
    expected = std::make_tuple<TRowId, std::optional<TRowData>>(3, std::nullopt);
    ASSERT_EQ(accepted[2], expected);
}

TEST(TDatabase, Test) {
    auto database = TDatabase::Create();

    ASSERT_EQ(database->Insert("table1", TRowData({1, "a"})).Succeeded, true);
    ASSERT_EQ(database->Insert("table1", TRowData({2, "b"})).Succeeded, true);
    ASSERT_EQ(database->Insert("table1", TRowData({3, "c"})).Succeeded, true);

    // call select asynchronously
    auto queue = MakeResultQueue();
    std::thread t([&](){
        ASSERT_TRUE(database->Select("table1", queue).Succeeded);
    });

    // first three messages contain data
    std::vector<TRowData> accepted;
    for (std::size_t i = 0; i < 3; i++) {
        auto val = queue->Pop();
        ASSERT_TRUE(std::holds_alternative<std::optional<NDatabase::TRowData>>(val));
        auto rowData = std::get<std::optional<NDatabase::TRowData>>(val);
        ASSERT_TRUE(rowData.has_value());
        accepted.emplace_back(std::move(*rowData));
    }

    // the last one contains the end mark
    auto val = queue->Pop();
    ASSERT_TRUE(std::holds_alternative<NDatabase::TEndOfTable>(val));

    std::sort(accepted.begin(), accepted.end(),
              [](const TRowData& lhs, const TRowData& rhs) { return lhs.Get<int>(0) < rhs.Get<int>(0); });

    ASSERT_EQ(accepted[0], TRowData({1, "a"}));
    ASSERT_EQ(accepted[1], TRowData({2, "b"}));
    ASSERT_EQ(accepted[2], TRowData({3, "c"}));

    t.join();
}

TEST(TThreadPool, Test) {
    TThreadPool<int> tp(2);
    std::array<std::future<int>, 2> futures;

    futures[0] = tp.Enqueue([](TThreadPool<int>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task1" << std::endl;
        return 0;
    });

    futures[1] = tp.Enqueue([](TThreadPool<int>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task2" << std::endl;
        return 1;
    });

    ASSERT_EQ(futures[0].get(), 0);
    ASSERT_EQ(futures[1].get(), 1);
}
