#include "parser.hpp"

#include <stdexcept>

namespace NBulk {
    void TParser::HandleLine(const std::string& line) {
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

} //namespace NBulk
