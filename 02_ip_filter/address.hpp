#ifndef IP_FILTER_ADDRESS_H
#define IP_FILTER_ADDRESS_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/types.h>

namespace NIPFilter {
    class TAddress {
    public:
        TAddress() = delete;

        TAddress(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
            : octets({p1, p2, p3, p4}){};

        TAddress(const std::string& in);

        friend std::ostream& operator<<(std::ostream& os, const TAddress& address);

        uint8_t GetOctet(std::size_t i) const;
        bool HasOctet(uint8_t value) const;
        bool operator==(const TAddress& other) const;
        bool operator<(const TAddress& other) const;

    private:
        std::array<uint8_t, 4> octets;

        void GetNextByte(const std::string& in, uint8_t& out, std::size_t& start);
    };

} //namespace NIPFilter

#endif
