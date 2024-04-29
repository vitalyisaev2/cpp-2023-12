#include "thread_pool.hpp"

namespace NUtils {

    TThreadPool::TThreadPool() {
        for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
            threads.push_back(MakeThread(i));
        }
    }

    TThreadPool::~TThreadPool() {
        for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
            queue.push(TTask{.Kind = TTask::EKind::Terminate});
        }

        for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
            threads[i].join();
        }
    }

    void TThreadPool::Enqueue(std::function<void(TThreadPool::TThreadId)> execution) {
        queue.push(TTask{.Kind = TTask::EKind::Execute, .Execute = std::move(execution)});
    }

    std::thread TThreadPool::MakeThread(TThreadPool::TThreadId threadId) {
        return std::thread{[&queue = this->queue, threadId] {
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
} //namespace NUtils
