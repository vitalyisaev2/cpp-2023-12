#ifndef IP_FILTER_ADDRESS_H
#define IP_FILTER_ADDRESS_H

#include <array>
#include <ostream>
#include <string>

namespace NIPFilter {
using TByte = unsigned short;
using TByteArray = std::array<TByte, 4>;

class TAddress : public TByteArray {
public:
    TAddress() = delete;

    TAddress(TByte p1, TByte p2, TByte p3, TByte p4): TByteArray({p1, p2, p3, p4}) {};

    TAddress(const std::string& in);

    bool operator<(const TAddress& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TAddress& address)
    {
        for (std::size_t i = 0; i < 4; i++) {
            os << address[i];
            if (i < 3)
                os << ".";
        }
        return os;
    }

private:
    void GetNextByte(const std::string& in, TByte& out, std::size_t& start);
};

}

#endif