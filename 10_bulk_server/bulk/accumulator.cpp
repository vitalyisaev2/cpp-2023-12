#include "accumulator.hpp"
#include <cstddef>
#include <mutex>

namespace NBulk {
    void TDefaultAccumulator::AddCommand(TCommand&& command) {
        // just save the command
        Buffer_.emplace_back(std::move(command));
    }

    void TDefaultAccumulator::Dump() {
        if (Buffer_.size()) {
            auto commands = std::make_shared<std::vector<TCommand>>(std::move(Buffer_));
            if (auto result = Printer_->HandleBlock(std::move(commands)).get(); !result.Ok) {
                throw std::runtime_error(result.Message);
            }
            // Clear the buffer after dumping to ensure it starts fresh for the next set of commands
            Buffer_.clear();
        }
    }

    void TThreadSafeGroupingAccumulator::AddCommand(TCommand&& command, std::size_t blockSize) {
        std::unique_lock<std::mutex> lock{Mutex_};

        Buffer_.emplace_back(std::move(command));

        // if enough data collected, dump it
        if (Buffer_.size() == blockSize) {
            auto commands = std::make_shared<std::vector<TCommand>>(std::move(Buffer_));
            if (auto result = Printer_->HandleBlock(std::move(commands)).get(); !result.Ok) {
                throw std::runtime_error(result.Message);
            }

            // Clear the buffer after dumping to ensure it starts fresh for the next set of commands
            Buffer_.clear();
        }
    }

    void TThreadSafeGroupingAccumulator::Dump() {
        std::unique_lock<std::mutex> lock{Mutex_};

        // if any data collected, dump it
        if (Buffer_.size()) {
            auto commands = std::make_shared<std::vector<TCommand>>(std::move(Buffer_));
            if (auto result = Printer_->HandleBlock(std::move(commands)).get(); !result.Ok) {
                throw std::runtime_error(result.Message);
            }
            // Clear the buffer after dumping to ensure it starts fresh for the next set of commands
            Buffer_.clear();
        }
    }
} //namespace NBulk
