#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "parser.hpp"
#include "printer.hpp"

// TAccumulatingPrinter keeps all comands in buffer. Useful for tests.
class TAccumulatingPrinter: public NBulk::IPrinter {
public:
    using TDump = std::vector<std::vector<std::string>>;

    void HandleBlock(const NBulk::TCommands& commands) override {
        Buffer.push_back(*commands);
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

void FillParser(NBulk::TParser& parser, const std::string& input) {
    std::string target;
    std::stringstream ss(input);

    while (std::getline(ss, target)) {
        parser.HandleLine(target);
    }

    parser.Terminate();
}

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
    auto threadPoolFile = NUtils::MakeThreadPool(2);
    auto threadPoolStdout = NUtils::MakeThreadPool(1);

    std::vector<NBulk::IPrinter::TPtr> lowLevelPrinters{
        std::make_shared<NBulk::TFilePrinter>(threadPoolFile),
        std::make_shared<NBulk::TStdOutPrinter>(threadPoolStdout),
    };

    NBulk::IPrinter::TPtr printer = std::make_shared<NBulk::TCompositePrinter>(std::move(lowLevelPrinters));
    NBulk::TParser parser(3, printer);

    const std::string input =
        R"(cmd1
    cmd2
    cmd3
    cmd4
    cmd5)";

    FillParser(parser, input);
}
