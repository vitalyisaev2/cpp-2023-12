#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

namespace NUtils {
    template <class T>
    class TThreadSafeQueue {
    public:
        void push(const T& value) {
            std::lock_guard<std::mutex> queueLock{QueueMutex};
            Queue.push(value);
            ConditionVariable.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> queueLock{QueueMutex};
            ConditionVariable.wait(queueLock, [&] { return !Queue.empty(); });
            T val = std::move(Queue.front());
            Queue.pop();
            return val;
        }

    private:
        std::queue<T> Queue;
        std::condition_variable ConditionVariable;
        std::mutex QueueMutex;
    };

    class TThreadPool {
    public:
        using TPtr = std::shared_ptr<TThreadPool>;

        using TThreadId = std::size_t;

    private:
        struct TTask {
            enum class EKind { Execute = 0, Terminate = 1 };

            EKind Kind;
            std::function<void(TThreadId)> Execute;
        };

        std::thread MakeThread(std::size_t threadId);

    public:
        explicit TThreadPool(std::size_t capacity);
        void Enqueue(std::function<void(TThreadId)> execution);

        ~TThreadPool();

    private:
        std::size_t Capacity;
        TThreadSafeQueue<TTask> Queue;
        std::vector<std::thread> Threads;
    };

    TThreadPool::TPtr MakeThreadPool(std::size_t capacity);
} //namespace NUtils
