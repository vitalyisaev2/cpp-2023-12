#include <memory>
#include <stdexcept>
#include <variant>

#include "state.hpp"

namespace NBulk {
    template <int BlockSize>
    IState::TPtr TNormalBlock<BlockSize>::HandleEvent(const TEvent& data) {
        if (std::holds_alternative<TCommand>(data)) {
            // accumulate commands in buffer
            Commands.emplace_back(std::move(std::get<TCommand>(data).value));

            // if enough data collected, dump it
            if (Commands.size() == BlockSize) {
                Printer->HandleBlock(std::move(Commands));
            }

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockStart>(data)) {
            // start dynamical block

            // dump collected data
            Printer->HandleBlock(std::move(Commands));

            // switch state
            return std::make_unique<TDynamicBlock<BlockSize>>();
        }

        if (std::holds_alternative<TBlockEnd>(data)) {
            throw std::invalid_argument("unexpected event of type TBlockEnd");
        }

        if (std::holds_alternative<TEndOfFile>(data)) {
            // print accumulated data
            Printer->HandleBlock(std::move(Commands));
        }

        throw std::invalid_argument("unexpected event");
    }

    template <int BlockSize>
    IState::TPtr TDynamicBlock<BlockSize>::HandleEvent(const TEvent& data) {
        if (std::holds_alternative<TCommand>(data)) {
            // just accumulate commands in buffer
            Commands.emplace_back(std::move(std::get<TCommand>(data).value));

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockStart>(data)) {
            // increment number of opened '{'s
            NestingLevel++;

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockEnd>(data)) {
            // decrement number of opened '{'s
            NestingLevel--;

            if (NestingLevel == 0) {
                // dump all data
                Printer->HandleBlock(std::move(Commands));

                return std::make_unique<TNormalBlock<BlockSize>>();
            }

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TEndOfFile>(data)) {
            // just do nothing
            return nullptr;
        }

        throw std::invalid_argument("unexpected event");
    }
} //namespace NBulk
