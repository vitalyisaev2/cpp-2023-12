#pragma once

#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <optional>

#include "thread_safe_queue.hpp"

namespace NDatabase {

    class TThreadPool {
    public:
        using TPtr = std::shared_ptr<TThreadPool>;

        static TThreadPool::TPtr Create(std::size_t capacity) {
            return std::make_shared<TThreadPool>(capacity);
        }

        using TThreadId = std::size_t;

    private:
        struct TTask {
            enum class EKind { Execute = 0, Terminate = 1 };

            EKind Kind;

            std::optional<std::function<void(TThreadId)>> Handler;
            std::optional<std::promise<void>> Promise;

            static TTask Termination() {
                return TTask{.Kind = EKind::Terminate, .Handler = std::nullopt, .Promise = std::nullopt};
            }
        };

        std::thread MakeThread(std::size_t threadId) {
            return std::thread{[&queue = this->Queue, threadId] {
                while (true) {
                    auto task = queue.Pop();
                    switch (task.Kind) {
                        case TTask::EKind::Execute:
                            (*task.Handler)(threadId);
                            task.Promise->set_value();
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

        std::future<void> Enqueue(std::function<void(TThreadId)> execution) {
            std::promise<void> promise;
            auto future = promise.get_future();
            Queue.Push(
                TTask{.Kind = TTask::EKind::Execute, .Handler = std::move(execution), .Promise = std::move(promise)});
            return future;
        }

        ~TThreadPool() {
            for (std::size_t i = 0; i < Capacity; i++) {
                Queue.Push(TTask::Termination());
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
} //namespace NDatabase
