#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <iostream>
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

        auto makeThread(std::size_t threadId) {
            return std::thread{[&queue = this->queue, threadId] {
                std::cout << "Thread " << threadId << ": start" << std::endl;
                while (true) {
                    const auto task = queue.pop();
                    switch (task.Kind) {
                        case TTask::EKind::Execute:
                            std::cout << "Thread " << threadId << ": execute" << std::endl;
                            task.Execute();
                            break;
                        case TTask::EKind::Terminate:
                            std::cout << "Thread " << threadId << ": terminate" << std::endl;
                            return;
                    }
                }
            }};
        };

    public:
        TThreadPool() {
            for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
                threads.push_back(makeThread(i));
            }
        }

        ~TThreadPool() {
            for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
                queue.push(TTask{.Kind = TTask::EKind::Terminate});
            }

            for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
                threads[i].join();
            }
        }

        void enqueue(std::function<void()> execution) {
            queue.push(TTask{.Kind = TTask::EKind::Execute, .Execute = std::move(execution)});
        }
    };

} //namespace real
