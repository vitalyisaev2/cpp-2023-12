#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "parser.hpp"
#include "printer.hpp"

void FillParser(NBulk::TParser& parser, const std::string& input) {
    std::string target;
    std::stringstream ss(input);

    while (std::getline(ss, target)) {
        parser.HandleLine(std::move(target));
    }

    parser.Terminate();
}

// TAccumulatingPrinter keeps all commands in buffer. Useful for tests.
class TAccumulatingPrinter: public NBulk::IPrinter {
public:
    using TDump = std::vector<std::vector<std::string>>;

    std::future<NBulk::IPrinter::TResult> HandleBlock(const NBulk::TCommands& commands) override {
        Buffer.push_back(*commands);
        std::promise<NBulk::IPrinter::TResult> promise;
        promise.set_value(NBulk::IPrinter::TResult{.Ok = true, .Message = ""});
        return promise.get_future();
    }

    void DumpResults(TDump& rcv) const {
        for (const auto& cmdBlock : Buffer) {
            std::vector<std::string> stringBlock;
            for (const auto& cmd : cmdBlock) {
                stringBlock.push_back(cmd.Value);
            }
            rcv.push_back(std::move(stringBlock));
        }
    };

private:
    std::vector<std::vector<NBulk::TCommand>> Buffer;
};

TEST(Parser, FakePrinter1) {
    auto printer = std::make_shared<TAccumulatingPrinter>();
    NBulk::TParser parser(3, printer);

    const std::string input =
        R"(cmd1
cmd2
cmd3
cmd4
cmd5)";

    FillParser(parser, input);

    const TAccumulatingPrinter::TDump expected = {
        {"cmd1", "cmd2", "cmd3"},
        {"cmd4", "cmd5"},
    };

    TAccumulatingPrinter::TDump actual;
    printer->DumpResults(actual);

    ASSERT_EQ(actual, expected);
}

TEST(Parser, FakePrinter2) {
    auto printer = std::make_shared<TAccumulatingPrinter>();
    NBulk::TParser parser(3, printer);

    const std::string input =
        R"(cmd1
cmd2
{
cmd3
cmd4
}
{
cmd5
cmd6
{
cmd7
cmd8
}
cmd9
}
{
cmd10
cmd11
)";

    FillParser(parser, input);

    const TAccumulatingPrinter::TDump expected = {
        {"cmd1", "cmd2"},
        {"cmd3", "cmd4"},
        {"cmd5", "cmd6", "cmd7", "cmd8", "cmd9"},
    };

    TAccumulatingPrinter::TDump actual;
    printer->DumpResults(actual);

    ASSERT_EQ(actual, expected);
}

TEST(Parser, RealPrinter) {
    auto threadPoolFile = NUtils::MakeThreadPool<NBulk::IPrinter::TResult>(2);
    auto threadPoolStdout = NUtils::MakeThreadPool<NBulk::IPrinter::TResult>(1);

    std::vector<NBulk::IPrinter::TPtr> lowLevelPrinters{
        std::make_shared<NBulk::TFilePrinter>(threadPoolFile),
        std::make_shared<NBulk::TStdOutPrinter>(threadPoolStdout),
    };

    NBulk::TParser parser(3, NBulk::MakeCompositePrinter(std::move(lowLevelPrinters)));

    const std::string input =
        R"(cmd1
    cmd2
    cmd3
    cmd4
    cmd5)";

    FillParser(parser, input);
}
