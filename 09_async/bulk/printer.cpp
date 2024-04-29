#include "printer.hpp"
#include "event.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

namespace NBulk {
    void TStdOutPrinter::HandleBlock(const std::vector<TCommand>& commands) {
        std::cout << "bulk: ";

        for (std::size_t i = 0; i < commands.size(); i++) {
            std::cout << commands[i].Value;

            if (i != commands.size() - 1) {
                std::cout << ", ";
            }
        }

        std::cout << std::endl;
    }

    void TFilePrinter::HandleBlock(const std::vector<TCommand>& commands) {
        const auto& startTime = commands.front().RegistrationTime;
        auto secondsUTC = std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count();

        auto filename =
            std::filesystem::current_path() / std::filesystem::path("bulk" + std::to_string(secondsUTC) + ".log");

        std::ofstream outfile(filename);

        for (const auto& cmd : commands) {
            outfile << cmd.Value << std::endl;
        }

        outfile.close();
    }

    void TAccumulatingPrinter::HandleBlock(const std::vector<TCommand>& commands) {
        // copy the whole block of commands to buffer
        Buffer.push_back(commands);
    }

    void TAccumulatingPrinter::DumpResults(TDump& rcv) const {
        for (const auto& cmdBlock : Buffer) {
            std::vector<std::string> stringBlock;
            for (const auto& cmd : cmdBlock) {
                stringBlock.push_back(cmd.Value);
            }
            rcv.push_back(std::move(stringBlock));
        }
    }

    void TCompositePrinter::HandleBlock(const std::vector<TCommand>& commands) {
        for (const auto& printer : Printers) {
            printer->HandleBlock(commands);
        }
    }

} //namespace NBulk
