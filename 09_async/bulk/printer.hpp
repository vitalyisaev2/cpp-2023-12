#pragma once

#include <memory>
#include <vector>

#include "event.hpp"

namespace NBulk {
    class IPrinter {
    public:
        using TPtr = std::shared_ptr<IPrinter>;
        virtual void HandleBlock(const std::vector<TCommand>& commands) = 0;
        virtual ~IPrinter(){};
    };

    // TStdOutPrinter just prints output into stdout.
    class TStdOutPrinter: public IPrinter {
    public:
        void HandleBlock(const std::vector<TCommand>& commands) override;
    };

    class TFilePrinter: public IPrinter {
    public:
        void HandleBlock(const std::vector<TCommand>& commands) override;
    };

    // TAccumulatingPrinter keeps all comands in buffer. Useful for tests.
    class TAccumulatingPrinter: public IPrinter {
    public:
        using TDump = std::vector<std::vector<std::string>>;

        void HandleBlock(const std::vector<TCommand>& commands) override;
        void DumpResults(TDump& rcv) const;

    private:
        std::vector<std::vector<TCommand>> Buffer;
    };

    class TCompositePrinter: public IPrinter {
    public:
        explicit TCompositePrinter(std::vector<IPrinter::TPtr> printers)
            : Printers(std::move(printers)) {
        }

        void HandleBlock(const std::vector<TCommand>& commands) override;

    private:
        std::vector<IPrinter::TPtr> Printers;
    };

    template <class P, class... Ps>
    void makeCompositePrinterStep(std::vector<IPrinter::TPtr>& printers) {
        printers.emplace_back(std::make_shared<P>());
        if constexpr (sizeof...(Ps) > 0) {
            return makeCompositePrinterStep<Ps...>(printers);
        }
    }

    template <class... Ps>
    IPrinter::TPtr MakeCompositePrinter() {
        std::vector<IPrinter::TPtr> printers;
        makeCompositePrinterStep<Ps...>(printers);
        return std::make_shared<TCompositePrinter>(printers);
    }

    template <class... Ps>
    IPrinter::TPtr MakeCompositePrinter();
} //namespace NBulk
