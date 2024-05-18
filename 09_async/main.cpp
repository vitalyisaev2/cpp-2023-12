#include <iostream>

#include "async.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "invalid number of args" << std::endl;
        exit(1);
    }

    try {
        std::size_t blockSize = std::stoi(argv[1]);

        auto handle = async::connect(blockSize);

        std::string line;
        while (std::getline(std::cin, line)) {
            async::receive(handle, line.data(), line.size());
        }

        async::disconnect(handle);
    } catch (std::exception& e) {
        std::cout << "Failure: " << e.what() << std::endl;
    }
}
