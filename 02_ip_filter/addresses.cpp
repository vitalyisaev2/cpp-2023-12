#include "addresses.hpp"

namespace NIPFilter {

TAddresses::TAddresses(std::istream& istream)
    : std::vector<TAddress>({})
{
    std::string line;
    while (std::getline(istream, line)) {
        this->emplace_back(ParseLine(line));
    }
}

void TAddresses::ReverseLexicographicSort()
{
    std::sort(this->rbegin(), this->rend());
}

TAddresses TAddresses::ApplyFilter(const TFilter& filter) const
{
    TAddresses out;
    std::copy_if(this->cbegin(), this->cend(), std::back_inserter(out), filter);
    return out;
}

TAddress TAddresses::ParseLine(const std::string& in)
{
    auto pos = in.find_first_of('\t');
    return TAddress(in.substr(0, pos));
}
}