#include "addresses.hpp"

namespace NIPFilter {
    TAddresses::TAddresses(std::istream& istream) {
        std::string line;
        while (std::getline(istream, line)) {
            this->items.emplace_back(ParseLine(line));
        }
    }

    void TAddresses::ReverseLexicographicSort() {
        std::sort(this->items.rbegin(), this->items.rend());
    }

    void TAddresses::Add(TAddress address) {
        this->items.emplace_back(std::move(address));
    }

    std::ostream& operator<<(std::ostream& os, const TAddresses& addresses) {
        for (const auto& address : addresses.items) {
            os << address << std::endl;
        }

        return os;
    }
    bool TAddresses::operator==(const TAddresses& other) const {
        return this->items == other.items;
    }

    TAddress TAddresses::ParseLine(const std::string& in) {
        auto pos = in.find_first_of('\t');
        return TAddress(in.substr(0, pos));
    }
} //namespace NIPFilter
