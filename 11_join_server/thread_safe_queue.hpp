#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace NDatabase {
    template <class T>
    class TThreadSafeQueue {
    public:
        using TPtr = std::shared_ptr<TThreadSafeQueue<T>>;

        void Push(T value) {
            std::unique_lock<std::mutex> lock{Mutex};
            Queue.push(std::move(value));
            ConditionVariable.notify_one();
        }

        T Pop() {
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

    template <class T>
    typename TThreadSafeQueue<T>::TPtr MakeThreadSafeQueue() {
        return std::make_shared<TThreadSafeQueue<T>>();
    }
} //namespace NDatabase
