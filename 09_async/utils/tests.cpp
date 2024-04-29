#include <gtest/gtest.h>

#include "thread_pool.hpp"

using namespace std::chrono_literals;

TEST(ThreadPool, Test) {
    NUtils::TThreadPool tp;

    tp.Enqueue([](NUtils::TThreadPool::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task1" << std::endl;
    });

    tp.Enqueue([](NUtils::TThreadPool::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task2" << std::endl;
    });
}
