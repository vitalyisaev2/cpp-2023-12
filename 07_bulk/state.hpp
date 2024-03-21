#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "printer.hpp"
#include "event.hpp"

namespace NBulk {

    std::ostream& operator<<(std::ostream& os, const TEvent& ev);

    // states
    class IState {
    public:
        using TPtr = std::unique_ptr<IState>;

        virtual IState::TPtr HandleEvent(const TEvent& data) = 0;
        virtual ~IState(){};
    };

    class TNormalBlock: public IState {
    public:
        explicit TNormalBlock(std::size_t blockSize, IPrinter::TPtr& printer)
            : BlockSize(blockSize)
            , Printer(printer){};

        IState::TPtr HandleEvent(const TEvent& data) override;

    private:
        std::size_t BlockSize;
        IPrinter::TPtr Printer;
        std::vector<TCommand> Commands;
    };

    class TDynamicBlock: public IState {
    public:
        TDynamicBlock(std::size_t blockSize, IPrinter::TPtr& printer) noexcept
            : BlockSize(blockSize)
            , NestingLevel(1)
            , Printer(printer){};

        IState::TPtr HandleEvent(const TEvent& data) override;

    private:
        std::size_t BlockSize;
        std::size_t NestingLevel;
        IPrinter::TPtr Printer;
        std::vector<TCommand> Commands;
    };
} //namespace NBulk
