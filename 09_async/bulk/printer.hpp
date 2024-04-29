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
        TStdOutPrinter() = delete;
        TStdOutPrinter(NUtils::TThreadPool::TPtr& threadPool)
            : ThreadPool(threadPool){};

        void HandleBlock(const TCommands& commands) override;

    private:
        NUtils::TThreadPool::TPtr ThreadPool;
    };

    class TFilePrinter: public IPrinter {
    public:
        TFilePrinter() = delete;
        TFilePrinter(NUtils::TThreadPool::TPtr& threadPool)
            : ThreadPool(threadPool){};

        void HandleBlock(const TCommands& commands) override;

    private:
        NUtils::TThreadPool::TPtr ThreadPool;
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

    IPrinter::TPtr MakeCompositePrinter(std::vector<IPrinter::TPtr>&& printers);
} //namespace NBulk
