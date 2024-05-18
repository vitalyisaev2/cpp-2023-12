#pragma once

#include <cstddef>
#include <memory>

#include "accumulator_factory.hpp"
#include "event.hpp"

namespace NBulk {

    std::ostream& operator<<(std::ostream& os, const TEvent& ev);

    // states
    class IState {
    public:
        using TPtr = std::unique_ptr<IState>;

        virtual IState::TPtr HandleEvent(TEvent data) = 0;
        virtual ~IState(){};
    };

    class TNormalBlock: public IState {
    public:
        explicit TNormalBlock(TAccumulatorFactory::TPtr accumulatorFactory)
            : AccumulatorFactory(accumulatorFactory){};

        IState::TPtr HandleEvent(TEvent data) override;

    private:
        TAccumulatorFactory::TPtr AccumulatorFactory;
    };

    class TDynamicBlock: public IState {
    public:
        TDynamicBlock(TAccumulatorFactory::TPtr accumulatorFactory)
            : NestingLevel(1)
            , AccumulatorFactory(accumulatorFactory)
            , Accumulator(accumulatorFactory->MakeDefaultAccumulator()){};

        IState::TPtr HandleEvent(TEvent data) override;

    private:
        std::size_t NestingLevel;
        TAccumulatorFactory::TPtr AccumulatorFactory;
        TDefaultAccumulator::TPtr Accumulator;
    };
} //namespace NBulk
