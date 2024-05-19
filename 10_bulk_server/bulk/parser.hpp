#pragma once

#include "printer.hpp"
#include "state.hpp"
#include <cstddef>
#include <memory>

namespace NBulk {
    class TParser {
    public:
        using TPtr = std::shared_ptr<TParser>;

        TParser(TAccumulatorFactory::TPtr accumulatorFactory, std::size_t blockSize) noexcept
            : State(std::make_unique<TNormalBlock>(accumulatorFactory, blockSize)) {
        }

        void HandleLine(std::string&& line);
        void Terminate();

    private:
        IState::TPtr State;
    };

    TParser::TPtr MakeParser(TAccumulatorFactory::TPtr accumulatorFactory, std::size_t blockSize);
} //namespace NBulk
