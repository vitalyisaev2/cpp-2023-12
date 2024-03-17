#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "printer.hpp"

namespace NBulk {
    // events
    struct TCommand {
        std::string value;
    };
    struct TBlockStart {};
    struct TBlockEnd {};
    struct TEndOfFile {};

    using TEvent = std::variant<TCommand, TBlockStart, TBlockEnd, TEndOfFile>;

    // states
    class IState {
    public:
        using TPtr = std::unique_ptr<IState>;

        virtual IState::TPtr HandleEvent(const TEvent& data) = 0;
        virtual ~IState() {};
    };

    template <int BlockSize>
    class TNormalBlock: public IState {
    public:
        IState::TPtr HandleEvent(const TEvent& data) override;

    private:
        TPrinter::TPtr Printer;
        std::vector<std::string> Commands;
    };

    template <int BlockSize>
    class TDynamicBlock: public IState {
    public:
        TDynamicBlock() noexcept
            : NestingLevel(1){};
        IState::TPtr HandleEvent(const TEvent& data) override;

    private:
        std::size_t NestingLevel;
        TPrinter::TPtr Printer;
        std::vector<std::string> Commands;
    };
} //namespace NBulk
