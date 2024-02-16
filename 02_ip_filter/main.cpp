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
        std::cout << src.ApplyFilter([](const NIPFilter::TAddress& address) { return address.GetOctet(0) == 1; });
    }

    // #3: first byte == 46, second byte == 70
    {
        std::cout << src.ApplyFilter(
            [](const NIPFilter::TAddress& address) { return address.GetOctet(0) == 46 && address.GetOctet(1) == 70; });
    }

    // #4: any byte is 46
    {
        std::cout << src.ApplyFilter([](const NIPFilter::TAddress& address) { return address.HasOctet(46); });
    }

    // #3:

    return 0;
}
