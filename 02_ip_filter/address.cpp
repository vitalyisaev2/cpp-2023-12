#include "address.hpp"

namespace NIPFilter {

    TAddress::TAddress(const std::string& in)
        : octets({}) {
        std::size_t start = 0;
        for (std::size_t i = 0; i < 4; i++) {
            GetNextByte(in, (*this).octets[i], start);
        }
    }

    uint8_t TAddress::GetOctet(std::size_t i) const {
        if (i > 3) {
            std::stringstream ss;
            ss << "Invalid octet index: " << std::to_string(i) << std::endl;
            throw ss.str();
        }

        return octets[i];
    }

    bool TAddress::HasOctet(uint8_t value) const {
        return std::find(octets.cbegin(), octets.cend(), value) != std::end(octets);
    };

    void TAddress::GetNextByte(const std::string& in, uint8_t& out, std::size_t& start) {
        auto stop = in.find_first_of('.', start);
        out = std::stoi(in.substr(start, stop - start));
        start = stop + 1;
    };

    bool TAddress::operator<(const TAddress& other) const {
        return this->octets < other.octets;
    }

    bool TAddress::operator==(const TAddress& other) const {
        return this->octets == other.octets;
    }

    std::ostream& operator<<(std::ostream& os, const TAddress& address) {
        for (std::size_t i = 0; i < 4; i++) {
            os << std::to_string(address.octets[i]);
            if (i < 3)
                os << ".";
        }
        return os;
    }

} // namespace NIPFilter
