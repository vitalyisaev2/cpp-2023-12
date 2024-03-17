#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "parser.hpp"
#include "printer.hpp"

void FillParser(NBulk::TParser& parser, const std::string& input) {
    std::string target;
    std::stringstream ss(input);

    while (std::getline(ss, target)) {
        parser.HandleLine(target);
    }

    parser.Terminate();
}

TEST(Parser, Test1) {
    auto printer = std::make_shared<NBulk::TAccumulatingPrinter>();
    NBulk::TParser parser(3, printer);

    const std::string input =
        R"(cmd1
cmd2
cmd3
cmd4
cmd5)";

    FillParser(parser, input);

    const NBulk::TAccumulatingPrinter::TDump expected = {
        {"cmd1", "cmd2", "cmd3"},
        {"cmd4", "cmd5"},
    };

    NBulk::TAccumulatingPrinter::TDump actual;
    printer->DumpResults(actual);

    ASSERT_EQ(actual, expected);
}

TEST(Parser, Test2) {
    auto printer = std::make_shared<NBulk::TAccumulatingPrinter>();
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

    const NBulk::TAccumulatingPrinter::TDump expected = {
        {"cmd1", "cmd2"},
        {"cmd3", "cmd4"},
        {"cmd5", "cmd6", "cmd7", "cmd8", "cmd9"},
    };

    NBulk::TAccumulatingPrinter::TDump actual;
    printer->DumpResults(actual);

    ASSERT_EQ(actual, expected);
}
