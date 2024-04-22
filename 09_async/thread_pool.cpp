#include "thread_pool.hpp"

#include <iostream>

namespace real {

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

    void TThreadPool::Enqueue(std::function<void()> execution) {
        queue.push(TTask{.Kind = TTask::EKind::Execute, .Execute = std::move(execution)});
    }

    std::thread TThreadPool::MakeThread(std::size_t threadId) {
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
} //namespace real
