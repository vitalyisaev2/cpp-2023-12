#pragma once

#include "printer.hpp"
#include "state.hpp"
#include <cstddef>
#include <memory>

namespace NBulk {
    class TParser {
    public:
        using TPtr = std::shared_ptr<TParser>;

        TParser(std::size_t blockSize, IPrinter::TPtr printer) noexcept
            : State(std::make_unique<TNormalBlock>(TNormalBlock(blockSize, printer))) {
        }

        void HandleLine(std::string&& line);
        void Terminate();

    private:
        IState::TPtr State;
    };

    TParser::TPtr MakeParser(std::size_t blockSize, IPrinter::TPtr&& printer);
} //namespace NBulk
