#include "state.hpp"
#include "event.hpp"

#include <memory>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <variant>

namespace NBulk {

    std::ostream& operator<<(std::ostream& os, const TEvent& ev) {
        if (std::holds_alternative<TCommand>(ev)) {
            os << "TCommand " << std::get<TCommand>(ev).Value;
        } else if (std::holds_alternative<TBlockStart>(ev)) {
            os << "TBlockStart";
        } else if (std::holds_alternative<TBlockEnd>(ev)) {
            os << "TBlockEnd";
        } else if (std::holds_alternative<TEndOfFile>(ev)) {
            os << "TEndOfFile";
        }

        return os;
    }

    IState::TPtr TNormalBlock::HandleEvent(TEvent ev) {
        if (std::holds_alternative<TCommand>(ev)) {
            // accumulate commands in buffer
            AccumulatorFactory_->GetThreadSafeGroupingAccumulator()->AddCommand(std::move(std::get<TCommand>(ev)), BlockSize_);

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockStart>(ev)) {
            // start new dynamical block

            // print accumulated data if any
            AccumulatorFactory_->GetThreadSafeGroupingAccumulator()->Dump();

            // switch state
            return std::make_unique<TDynamicBlock>(AccumulatorFactory_, BlockSize_);
        }

        if (std::holds_alternative<TBlockEnd>(ev)) {
            throw std::invalid_argument("TNormalBlock::HandleEvent: unexpected event of type TBlockEnd");
        }

        if (std::holds_alternative<TEndOfFile>(ev)) {
            // We used to Dump accumulated commands before, but now this buffer is shared among threads,
            // so there's no sense to dump it as someone else may use it.

            // AccumulatorFactory_->GetThreadSafeGroupingAccumulator()->Dump();

            // no state change
            return nullptr;
        }

        throw std::invalid_argument("TNormalBlock::HandleEvent: unexpected event");
    };

    IState::TPtr TDynamicBlock::HandleEvent(TEvent ev) {
        if (std::holds_alternative<TCommand>(ev)) {
            // just accumulate commands in buffer
            DefaultAccumulator_->AddCommand(std::move(std::get<TCommand>(ev)));

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockStart>(ev)) {
            // increment number of opened '{'s
            NestingLevel_++;

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TBlockEnd>(ev)) {
            // decrement number of opened '{'s
            NestingLevel_--;

            if (NestingLevel_ == 0) {
                // dump all data
                DefaultAccumulator_->Dump();

                return std::make_unique<TNormalBlock>(AccumulatorFactory_, BlockSize_);
            }

            // no state change
            return nullptr;
        }

        if (std::holds_alternative<TEndOfFile>(ev)) {
            // just do nothing
            return nullptr;
        }

        throw std::invalid_argument("TDynamicBlock::HandleEvent: unexpected event");
    };

} //namespace NBulk
