#include <cstddef>
#include <memory>
#include <mutex>

#include "printer.hpp"

namespace NBulk {


    class TDefaultAccumulator{
    public:
        // This kind of accumulator should always recide in a certain thread
        using TPtr = std::unique_ptr<TDefaultAccumulator>;

        TDefaultAccumulator(IPrinter::TPtr printer)
            : Printer_(printer) {
        }

        void AddCommand(TCommand&& command) ;
        void Dump() ;

    private:
        std::vector<TCommand> Buffer_;
        IPrinter::TPtr Printer_;
    };

    class TThreadSafeGroupingAccumulator{
    public:
        // This kind of accumulator is usually shared accross threads.
        using TPtr = std::shared_ptr<TThreadSafeGroupingAccumulator>;

        TThreadSafeGroupingAccumulator(IPrinter::TPtr printer)
            : Printer_(printer) {
        }

        void AddCommand(TCommand&& command, std::size_t blockSize) ;
        void Dump() ;

    private:
        std::mutex Mutex_;
        std::vector<TCommand> Buffer_;
        IPrinter::TPtr Printer_;
    };

} //namespace NBulk
