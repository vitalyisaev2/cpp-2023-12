#pragma once

#include <mutex>
#include <thread>
#include <iostream>
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

    template <class T>
    struct TTask {
        enum class EKind { Normal = 0, Terminate = 1 };

        T Payload;
        EKind Kind;
    };

    class TThreadPool {
        // void consumer() {
        //     std::unique_lock<std::mutex> lck{conditionMutex};

        //     while (!finished) {
        //         condition.wait(lck);

        //         auto task = std::move(queue.front());
        //         queue.pop();

        //         std::cout << "Consumer - next data to proceed!\n";
        //         std::cout << task << std::endl;
        //     }

        //     std::cout << "Consumer - finished!" << std::endl;
        // }

    public:
        TThreadPool() {
            for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
                threads.push_back(std::thread(consumer));
            }
        }

        void publish(int value) {
            std::lock_guard<std::mutex> guard{conditionMutex};
            queue.push(value);
            condition.notify_one();
        }

    private:
        std::vector<std::jthread> threads;
    };

} //namespace real
