#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>


namespace NDatabase {
    template <class T>
    class TThreadSafeQueue {
    public:
        void push(T value) {
            std::unique_lock<std::mutex> lock{Mutex};
            Queue.push(std::move(value));
            ConditionVariable.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock{Mutex};
            ConditionVariable.wait(lock, [&] { return !Queue.empty(); });
            T val = std::move(Queue.front());
            Queue.pop();
            return val;
        }

    private:
        std::queue<T> Queue;
        std::condition_variable ConditionVariable;
        std::mutex Mutex;
    };
}
