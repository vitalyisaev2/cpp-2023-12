#ifndef IP_FILTER_ADDRESSES_H
#define IP_FILTER_ADDRESSES_H

#include <functional>
#include <istream>
#include <vector>

#include "address.hpp"

namespace NIPFilter {

    class TAddresses {
    public:
        TAddresses(){};
        TAddresses(std::istream& istream);

        void Add(TAddress address);

        void ReverseLexicographicSort();

        template <class TFilter>
        TAddresses ApplyFilter(const TFilter& filter) const {
            TAddresses out;
            std::copy_if(this->items.cbegin(), this->items.cend(), std::back_inserter(out.items), filter);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& os, const TAddresses& addresses);

        bool operator==(const TAddresses& other) const;

    private:
        std::vector<TAddress> items;
        TAddress ParseLine(const std::string& in);
    };
} //namespace NIPFilter

#endif
