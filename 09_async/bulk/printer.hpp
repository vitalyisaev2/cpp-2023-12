#pragma once

#include <memory>
#include <vector>

#include "event.hpp"
#include "../utils/thread_pool.hpp"

namespace NBulk {
    class IPrinter {
    public:
        using TPtr = std::shared_ptr<IPrinter>;
        virtual void HandleBlock(const TCommands& commands) = 0;
        virtual ~IPrinter(){};
    };

    // TStdOutPrinter just prints output into stdout.
    class TStdOutPrinter: public IPrinter {
    public:
        void HandleBlock(const TCommands& commands) override;
    
    private:
        NUtils::TThreadPool ThreadPool;
    };

    class TFilePrinter: public IPrinter {
    public:
        void HandleBlock(const TCommands& commands) override;

    private:
        NUtils::TThreadPool ThreadPool;
    };


    class TCompositePrinter: public IPrinter {
    public:
        explicit TCompositePrinter(std::vector<IPrinter::TPtr> printers)
            : Printers(std::move(printers)) {
        }

        void HandleBlock(const TCommands& commands) override;

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
