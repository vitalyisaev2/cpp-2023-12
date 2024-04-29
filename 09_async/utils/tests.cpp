#include <chrono>

#include <gtest/gtest.h>

#include "thread_pool.hpp"

using namespace std::chrono_literals;


TEST(ThreadPool, Test) {
    NUtils::TThreadPool tp;

    // std::this_thread::sleep_for(100ms);

    tp.Enqueue([]() { std::cout << "task1" << std::endl; });
    tp.Enqueue([]() { std::cout << "task2" << std::endl; });

    // std::this_thread::sleep_for(100ms);
}
