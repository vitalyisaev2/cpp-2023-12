#include "printer.hpp"
#include "event.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace NBulk {
    void TStdOutPrinter::HandleBlock(const TCommands& commands) {
        ThreadPool->Enqueue([commands = commands](NUtils::TThreadPool::TThreadId _) {
            std::cout << "bulk: ";

            for (std::size_t i = 0; i < commands->size(); i++) {
                std::cout << commands->at(i).Value;

                if (i != commands->size() - 1) {
                    std::cout << ", ";
                }
            }

            std::cout << std::endl;
        });
    }

    void TFilePrinter::HandleBlock(const TCommands& commands) {
        ThreadPool->Enqueue([commands = commands](NUtils::TThreadPool::TThreadId threadId) {
            const auto& startTime = commands->front().RegistrationTime;
            auto secondsUTC = std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count();

            auto filename_local =
                std::filesystem::path("bulk" + std::to_string(secondsUTC) + "_" + std::to_string(threadId) + ".log");
            auto filename_abs = std::filesystem::current_path() / filename_local;

            std::ofstream outfile(filename_abs);
            for (const auto& cmd : *commands) {
                outfile << cmd.Value << std::endl;
            }

            outfile.close();
        });
    }

    void TCompositePrinter::HandleBlock(const TCommands& commands) {
        for (const auto& printer : Printers) {
            printer->HandleBlock(commands);
        }
    }
} //namespace NBulk
