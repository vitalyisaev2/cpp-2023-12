#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

namespace real {
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
    private:
        struct TTask {
            enum class EKind { Execute = 0, Terminate = 1 };

            EKind Kind;
            std::function<void()> Execute;
        };

        TThreadSafeQueue<TTask> queue;
        std::vector<std::thread> threads;

        std::thread MakeThread(std::size_t threadId);

    public:
        TThreadPool();
        ~TThreadPool();
        void Enqueue(std::function<void()> execution);
    };

} //namespace real
