#pragma once

#include <string>
#include <chrono>
#include <variant>

namespace NBulk {
    struct TCommand {
        TCommand() = delete;

        explicit TCommand(std::string value) noexcept
            : Value(std::move(value))
            , RegistrationTime(std::chrono::system_clock::now()) {
        }

        std::string Value;
        std::chrono::time_point<std::chrono::system_clock> RegistrationTime;
    };

    struct TBlockStart {};

    struct TBlockEnd {};

    struct TEndOfFile {};

    using TEvent = std::variant<TCommand, TBlockStart, TBlockEnd, TEndOfFile>;
} //namespace NBulk
