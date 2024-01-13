#ifndef IP_FILTER_ADDRESSES_H
#define IP_FILTER_ADDRESSES_H

#include <algorithm>
#include <functional>
#include <istream>
#include <vector>

#include "address.hpp"

namespace NIPFilter {

class TAddresses : public std::vector<TAddress> {
public:
    using TFilter = std::function<bool(const TAddress&)>;

    TAddresses()
        : std::vector<TAddress>({})
    {
    }

    TAddresses(std::istream& istream);

    void ReverseLexicographicSort();
    TAddresses ApplyFilter(const TFilter& filter) const;

    friend std::ostream& operator<<(std::ostream& os, const TAddresses& addresses)
    {
        for (const auto& address : addresses) {
            os << address << std::endl;
        }

        return os;
    }

private:
    TAddress ParseLine(const std::string& in);
};
}

#endif