#include <iostream>

#include "parser.hpp"
#include "printer.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "invalid number of args" << std::endl;
        exit(1);
    }

    std::size_t blockSize = std::stoi(argv[1]);

    NBulk::IPrinter::TPtr printer = NBulk::MakeCompositePrinter<NBulk::TStdOutPrinter, NBulk::TFilePrinter>();
    NBulk::TParser parser(blockSize, printer);

    std::string line;
    while (std::getline(std::cin, line)) {
        parser.HandleLine(line);
    }

    parser.Terminate();
}
