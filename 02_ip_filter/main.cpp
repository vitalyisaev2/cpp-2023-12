#include <iostream>

#include "addresses.hpp"

int main()
{
    auto src = NIPFilter::TAddresses(std::cin);

    // #1: print reverse sorted list
    src.ReverseLexicographicSort();
    std::cout << src;

    // #2: first byte == 1
    {
        std::cout << src.ApplyFilter([](const NIPFilter::TAddress& address) { return address[0] == 1; });
    }

    // #3: first byte == 46, second byte == 70
    {
        std::cout << src.ApplyFilter([](const NIPFilter::TAddress& address) {
            return address[0] == 46 && address[1] == 70;
        });
    }

    // #4: any byte is 46
    {
        std::cout << src.ApplyFilter([](const NIPFilter::TAddress& address) {
            auto result = std::find(address.cbegin(), address.cend(), NIPFilter::TByte(46));
            return result != std::end(address);
        });
    }

    // #3:

    return 0;
}