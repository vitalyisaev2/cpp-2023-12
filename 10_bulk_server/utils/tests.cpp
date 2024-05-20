#include <gtest/gtest.h>

#include "thread_pool.hpp"

TEST(ThreadPool, Test) {
    NUtils::TThreadPool<int> tp(2);
    std::array<std::future<int>, 2> futures;

    futures[0] = tp.Enqueue([](NUtils::TThreadPool<int>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task1" << std::endl;
        return 0;
    });

    futures[1] = tp.Enqueue([](NUtils::TThreadPool<int>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task2" << std::endl;
        return 1;
    });

    ASSERT_EQ(futures[0].get(), 0);
    ASSERT_EQ(futures[1].get(), 1);
}
