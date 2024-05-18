#include <cstddef>
#include <memory>
#include <mutex>

#include "printer.hpp"

namespace NBulk {

    class IAccumulator {
    public:
        virtual void AddCommand(TCommand&& command) = 0;
        virtual void Dump() = 0;
        virtual ~IAccumulator() = default; // Add a virtual destructor
    };

    class TDefaultAccumulator: public IAccumulator {
    public:
        // This kind of accumulator should always recide in a certain thread
        using TPtr = std::unique_ptr<IAccumulator>;

        TDefaultAccumulator(IPrinter::TPtr printer)
            : Printer_(printer) {
        }

        void AddCommand(TCommand&& command) override;
        void Dump() override;

    private:
        std::vector<TCommand> Buffer_;
        IPrinter::TPtr Printer_;
    };

    class TThreadSafeGroupingAccumulator: public IAccumulator {
    public:
        // This kind of accumulator is usually shared accross threads.
        using TPtr = std::shared_ptr<IAccumulator>;

        TThreadSafeGroupingAccumulator(std::size_t blockSize, IPrinter::TPtr printer)
            : BlockSize_(blockSize)
            , Printer_(printer) {
        }

        void AddCommand(TCommand&& command) override;
        void Dump() override;

    private:
        const std::size_t BlockSize_;
        std::mutex Mutex_;
        std::vector<TCommand> Buffer_;
        IPrinter::TPtr Printer_;
    };

} //namespace NBulk
