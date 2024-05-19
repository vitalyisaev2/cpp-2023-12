#include <boost/asio/detail/assert.hpp>
#include <cstddef>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

#include "async.hpp"
#include "bulk/parser.hpp"
#include "bulk/printer.hpp"
#include "bulk/state.hpp"
#include "utils/thread_pool.hpp"

// TParserController is a singleton container type providing instances of thread pool.
class TParserController {
public:
    static TParserController* GetInstance();

    using TParserId = std::size_t;

private:
    TParserController(NUtils::TThreadPool<NBulk::IPrinter::TResult>::TPtr&& threadPoolFile,
                      NUtils::TThreadPool<NBulk::IPrinter::TResult>::TPtr&& threadPoolStdOut)
        : File(std::move(threadPoolFile))
        , StdOut(std::move(threadPoolStdOut))
        , ParserCounter(0) {

        std::vector<NBulk::IPrinter::TPtr> lowLevelPrinters{
            std::make_shared<NBulk::TFilePrinter>(File),
            std::make_shared<NBulk::TStdOutPrinter>(StdOut),
        };

        AccumulatorFactory = NBulk::MakeAccumulatorFactory(NBulk::MakeCompositePrinter(std::move(lowLevelPrinters)));
    }

    static std::atomic<TParserController*> Instance;
    static std::mutex InstanceMutex;
    NUtils::TThreadPool<NBulk::IPrinter::TResult>::TPtr File;
    NUtils::TThreadPool<NBulk::IPrinter::TResult>::TPtr StdOut;
    NBulk::TAccumulatorFactory::TPtr AccumulatorFactory;

    std::unordered_map<TParserId, NBulk::TParser::TPtr> Parsers;
    TParserId ParserCounter;
    std::shared_mutex StorageMutex; // synchronizes access to storage

public:
    TParserId MakeParser(const std::size_t blockSize) {
        std::unique_lock<std::shared_mutex> lock(StorageMutex);

        auto out = ParserCounter;

        Parsers[ParserCounter++] = NBulk::MakeParser(AccumulatorFactory, blockSize);

        return out;
    }

    void HandleLine(const TParserId parserId, std::string&& line) {
        std::shared_lock<std::shared_mutex> lock(StorageMutex);
        Parsers[parserId]->HandleLine(std::move(line));
    }

    void RemoveParser(const TParserId parserId) {
        std::unique_lock<std::shared_mutex> lock(StorageMutex);

        auto iter = Parsers.find(parserId);
        if (iter != Parsers.end()) {
            iter->second->Terminate();
            Parsers.erase(iter);
        }
    }
};

std::atomic<TParserController*> TParserController::Instance{nullptr};
std::mutex TParserController::InstanceMutex;

TParserController* TParserController::GetInstance() {
    if (Instance == nullptr) {
        std::lock_guard<std::mutex> lock(InstanceMutex);
        if (Instance == nullptr) {
            Instance = new TParserController(NUtils::MakeThreadPool<NBulk::IPrinter::TResult>(2),
                                             NUtils::MakeThreadPool<NBulk::IPrinter::TResult>(1));
        }
    }

    return Instance;
}

namespace async {

    handle_t connect(std::size_t bulk) {
        return reinterpret_cast<handle_t>(TParserController::GetInstance()->MakeParser(bulk));
    }

    void receive(handle_t handle, std::string&& input) {
        TParserController::GetInstance()->HandleLine(reinterpret_cast<TParserController::TParserId>(handle),
                                                     std::move(input));
    }

    void disconnect(handle_t handle) {
        TParserController::GetInstance()->RemoveParser(reinterpret_cast<TParserController::TParserId>(handle));
    }

} //namespace async
