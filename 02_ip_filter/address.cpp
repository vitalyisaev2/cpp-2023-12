#include "address.hpp"

#include <iostream>

namespace NIPFilter {

TAddress::TAddress(const std::string& in)
    : TByteArray()
{
    std::size_t start = 0;
    for (std::size_t i = 0; i < 4; i++) {
        GetNextByte(in, (*this)[i], start);
    }
}

bool TAddress::operator<(const TAddress& other) const
{
    for (std::size_t i = 0; i < 4; i++) {
        if ((*this)[i] < other[i]) {
            return true;
        }

        if ((*this)[i] > other[i]) {
            return false;
        }
    };

    return false;
}

void TAddress::GetNextByte(const std::string& in, TByte& out, std::size_t& start)
{
    auto stop = in.find_first_of('.', start);
    out = std::stoi(in.substr(start, stop - start));
    start = stop + 1;
};
} // namespace NIPFilter