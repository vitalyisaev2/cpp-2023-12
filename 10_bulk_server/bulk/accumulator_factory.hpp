#include "accumulator.hpp"
#include "printer.hpp"
#include <memory>

namespace NBulk {
    class TAccumulatorFactory {
    public:
        using TPtr = std::shared_ptr<TAccumulatorFactory>;

        TAccumulatorFactory(IPrinter::TPtr printer)
            : Printer_(printer)
            , ThreadSafeGroupingAccumulator_(std::make_shared<TThreadSafeGroupingAccumulator>(printer)) {
        }

        TThreadSafeGroupingAccumulator::TPtr GetThreadSafeGroupingAccumulator() const {
            // share the same instance
            return ThreadSafeGroupingAccumulator_;
        }

        TDefaultAccumulator::TPtr MakeDefaultAccumulator() const {
            return std::make_unique<TDefaultAccumulator>(Printer_);
        }

    private:
        IPrinter::TPtr Printer_;
        TThreadSafeGroupingAccumulator::TPtr ThreadSafeGroupingAccumulator_;
    };

    TAccumulatorFactory::TPtr MakeAccumulatorFactory(IPrinter::TPtr printer);
} //namespace NBulk
