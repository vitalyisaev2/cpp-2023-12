#include "address.hpp"

namespace NIPFilter {

    TAddress::TAddress(const std::string& in)
        : octets({})
    {
        std::size_t start = 0;
        for (std::size_t i = 0; i < 4; i++)
        {
            GetNextByte(in, (*this).octets[i], start);
        }
    }

    bool TAddress::operator<(const TAddress& other) const {
        for (std::size_t i = 0; i < 4; i++)
        {
            if (this->octets[i] < other.octets[i])
            {
                return true;
            }

            if (this->octets[i] > other.octets[i])
            {
                return false;
            }
        };

        return false;
    }

    void TAddress::GetNextByte(const std::string& in, uint8_t& out, std::size_t& start)
    {
        auto stop = in.find_first_of('.', start);
        out = std::stoi(in.substr(start, stop - start));
        start = stop + 1;
    };
} // namespace NIPFilter
