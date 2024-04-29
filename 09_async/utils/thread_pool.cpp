#include "thread_pool.hpp"

namespace NUtils {

    TThreadPool::TThreadPool(std::size_t capacity)
        : Capacity(capacity) {
        for (std::size_t i = 0; i < Capacity; i++) {
            Threads.push_back(MakeThread(i));
        }
    }

    TThreadPool::~TThreadPool() {
        for (std::size_t i = 0; i < Capacity; i++) {
            Queue.push(TTask{.Kind = TTask::EKind::Terminate});
        }

        for (std::size_t i = 0; i < Capacity; i++) {
            Threads[i].join();
        }
    }

    void TThreadPool::Enqueue(std::function<void(TThreadPool::TThreadId)> execution) {
        Queue.push(TTask{.Kind = TTask::EKind::Execute, .Execute = std::move(execution)});
    }

    std::thread TThreadPool::MakeThread(TThreadPool::TThreadId threadId) {
        return std::thread{[&queue = this->Queue, threadId] {
            while (true) {
                const auto task = queue.pop();
                switch (task.Kind) {
                    case TTask::EKind::Execute:
                        task.Execute(threadId);
                        break;
                    case TTask::EKind::Terminate:
                        return;
                }
            }
        }};
    };

    TThreadPool::TPtr MakeThreadPool(std::size_t capacity) {
        return std::make_shared<TThreadPool>(capacity);
    }
} //namespace NUtils
