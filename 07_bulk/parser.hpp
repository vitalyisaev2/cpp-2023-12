#pragma once

#include "printer.hpp"
#include "state.hpp"
#include <cstddef>
#include <memory>

namespace NBulk {
    class TParser {
    public:
        explicit TParser(std::size_t blockSize, IPrinter::TPtr printer) noexcept
            : State(std::make_unique<TNormalBlock>(TNormalBlock(blockSize, printer))) {
        }

        void HandleLine(const std::string& line);
        void Terminate();

    private:
        IState::TPtr State;
    };
} //namespace NBulk
