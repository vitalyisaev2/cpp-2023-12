#pragma once

#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <thread>

#include "thread_safe_queue.hpp"

namespace NDatabase {

    template <class T>
    class TThreadPool {
    public:
        using TPtr = std::shared_ptr<TThreadPool>;

        using TThreadId = std::size_t;

    private:
        struct TTask {
            enum class EKind { Execute = 0, Terminate = 1 };

            EKind Kind;

            std::function<T(TThreadId)> Execute;
            std::promise<T> Promise;
        };

        std::thread MakeThread(std::size_t threadId) {
            return std::thread{[&queue = this->Queue, threadId] {
                while (true) {
                    auto task = queue.pop();
                    switch (task.Kind) {
                        case TTask::EKind::Execute:
                            task.Promise.set_value(task.Execute(threadId));
                            break;
                        case TTask::EKind::Terminate:
                            return;
                    }
                }
            }};
        }

    public:
        TThreadPool() = delete;

        explicit TThreadPool(std::size_t capacity)
            : Capacity(capacity) {
            for (std::size_t i = 0; i < Capacity; i++) {
                Threads.push_back(MakeThread(i));
            }
        };

        std::future<T> Enqueue(std::function<T(TThreadId)> execution) {
            std::promise<T> promise;
            auto future = promise.get_future();
            Queue.push(
                TTask{.Kind = TTask::EKind::Execute, .Execute = std::move(execution), .Promise = std::move(promise)});
            return future;
        }

        ~TThreadPool() {
            for (std::size_t i = 0; i < Capacity; i++) {
                Queue.push(TTask{.Kind = TTask::EKind::Terminate});
            }

            for (std::size_t i = 0; i < Capacity; i++) {
                Threads[i].join();
            }
        }

    private:
        std::size_t Capacity;
        TThreadSafeQueue<TTask> Queue;
        std::vector<std::thread> Threads;
    };

    template <class T>
    typename TThreadPool<T>::TPtr MakeThreadPool(std::size_t capacity) {
        return std::make_shared<TThreadPool<T>>(capacity);
    }
} //namespace NDatabase
