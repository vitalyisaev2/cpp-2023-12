#include <thread>

#include <gtest/gtest.h>

#include "async.hpp"

void FillParser(async::handle_t& handle, const std::string& input) {
    std::string target;
    std::stringstream ss(input);

    while (std::getline(ss, target)) {
        async::receive(handle, target.data(), target.size());
    }
}

TEST(Async, Test) {
    auto t1 = std::thread([] {
        std::size_t blockSize = 5;

        const std::string input =
            R"(cmd1
cmd2
cmd3
cmd4
cmd5)";

        auto handle = async::connect(blockSize);
        FillParser(handle, input);
        async::disconnect(handle);
    });

    auto t2 = std::thread([] {
        std::size_t blockSize = 5;

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

        auto handle = async::connect(blockSize);
        FillParser(handle, input);
        async::disconnect(handle);
    });

    t1.join();
    t2.join();
}