#include "thread_pool.hpp"

namespace real {
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
