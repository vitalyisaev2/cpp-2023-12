#include "accumulator_factory.hpp"

namespace NBulk {
    TAccumulatorFactory::TPtr MakeAccumulatorFactory(IPrinter::TPtr printer) {
        return std::make_shared<TAccumulatorFactory>(printer);
    }
} //namespace NBulk
