#include <gtest/gtest.h>

#include "thread_pool.hpp"

using namespace std::chrono_literals;

TEST(ThreadPool, Test) {
    NUtils::TThreadPool<void> tp(2);
    std::vector<std::future<void>> futures(2);

    futures[0] = tp.Enqueue([](NUtils::TThreadPool<void>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task1" << std::endl;
    });

    futures[1] = tp.Enqueue([](NUtils::TThreadPool<void>::TThreadId threadId) {
        std::cout << "threadId = " << threadId << ", " << "task2" << std::endl;
    });

    for (const auto& f: futures) {
        f.wait();
    }
}
