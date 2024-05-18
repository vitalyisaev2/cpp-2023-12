#include "parser.hpp"
#include "state.hpp"

#include <memory>
#include <stdexcept>

namespace NBulk {
    void TParser::HandleLine(std::string&& line) {
        IState::TPtr newState = nullptr;

        if (line == "{") {
            newState = State->HandleEvent(TBlockStart{});
        } else if (line == "}") {
            newState = State->HandleEvent(TBlockEnd{});
        } else if (line.size() > 0) {
            newState = State->HandleEvent(TCommand(std::move(line)));
        } else {
            throw std::invalid_argument("unexpected input");
        }

        if (newState) {
            State = std::move(newState);
        }
    }

    void TParser::Terminate() {
        State->HandleEvent(TEndOfFile{});
    }

    TParser::TPtr MakeParser(TAccumulatorFactory::TPtr accumulatorFactory) {
        return std::make_shared<TParser>(std::move(accumulatorFactory));
    }
} //namespace NBulk
