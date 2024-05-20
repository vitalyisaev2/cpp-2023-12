#include "printer.hpp"
#include "event.hpp"

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <memory>

namespace NBulk {
    void DumpCommands(std::ostream& stream, const TCommands& commands) {
        stream << "bulk: ";

        for (std::size_t i = 0; i < commands->size(); i++) {
            stream << commands->at(i).Value;

            if (i != commands->size() - 1) {
                stream << ", ";
            }
        }

        stream << std::endl;
    }

    std::future<IPrinter::TResult> TStdOutPrinter::HandleBlock(const TCommands& commands) {
        return ThreadPool->Enqueue(
            [commands =
                 commands]([[maybe_unused]] NUtils::TThreadPool<IPrinter::TResult>::TThreadId threadId) -> TResult {
                DumpCommands(std::cout, commands);
                return TResult{.Ok = true, .Message = ""};
            });
    }

    std::future<IPrinter::TResult> TFilePrinter::HandleBlock(const TCommands& commands) {
        return ThreadPool->Enqueue([commands = commands](
                                       NUtils::TThreadPool<IPrinter::TResult>::TThreadId threadId) -> TResult {
            const auto& startTime = commands->front().RegistrationTime;
            auto secondsUTC = std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch()).count();

            auto filenameLocal =
                std::filesystem::path("bulk" + std::to_string(secondsUTC) + "_" + std::to_string(threadId) + ".log");
            auto filenameAbs = std::filesystem::current_path() / filenameLocal;

            std::ofstream outfile(filenameAbs);

            DumpCommands(outfile, commands);

            outfile.close();

            // TODO: Check error codes from IO operations
            return TResult{.Ok = true, .Message = ""};
        });
    }

    std::future<IPrinter::TResult> TCompositePrinter::HandleBlock(const TCommands& commands) {
        std::vector<std::future<TResult>> futures;
        futures.reserve(Printers.size());

        for (const auto& printer : Printers) {
            futures.emplace_back(printer->HandleBlock(commands));
        }

        IPrinter::TResult result;
        std::size_t i = 0;

        do {
            result = futures[i].get();
            if (!result.Ok) {
                // fail fast
                break;
            }
            i++;
        } while (i < futures.size());

        // Copy last cached result to the outgoing future;
        std::promise<IPrinter::TResult> promise;
        promise.set_value(result);

        return promise.get_future();
    }

    IPrinter::TPtr MakeCompositePrinter(std::vector<IPrinter::TPtr>&& printers) {
        return std::make_shared<TCompositePrinter>(printers);
    }
} //namespace NBulk
