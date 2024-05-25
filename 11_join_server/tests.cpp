#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

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

TEST(ThreadPool, Test) {
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
