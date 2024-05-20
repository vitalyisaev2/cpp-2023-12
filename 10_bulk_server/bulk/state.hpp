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
        explicit TNormalBlock(TAccumulatorFactory::TPtr accumulatorFactory, std::size_t blockSize)
            : AccumulatorFactory_(accumulatorFactory), BlockSize_(blockSize){};

        IState::TPtr HandleEvent(TEvent data) override;

    private:
        TAccumulatorFactory::TPtr AccumulatorFactory_;
        std::size_t BlockSize_;
    };

    class TDynamicBlock: public IState {
    public:
        TDynamicBlock(TAccumulatorFactory::TPtr accumulatorFactory, std::size_t blockSize)
            : NestingLevel_(1)
            , BlockSize_(blockSize)
            , AccumulatorFactory_(accumulatorFactory)
            , DefaultAccumulator_(accumulatorFactory->MakeDefaultAccumulator()){};

        IState::TPtr HandleEvent(TEvent data) override;

    private:
        std::size_t NestingLevel_;
        std::size_t BlockSize_;
        TAccumulatorFactory::TPtr AccumulatorFactory_;
        TDefaultAccumulator::TPtr DefaultAccumulator_;
    };
} //namespace NBulk
