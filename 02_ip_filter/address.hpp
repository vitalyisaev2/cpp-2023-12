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
            : octets({p1, p2, p3, p4})
                  {};

        TAddress(const std::string& in);

        bool operator<(const TAddress& other) const;

        friend std::ostream& operator<<(std::ostream& os, const TAddress& address)
        {
            for (std::size_t i = 0; i < 4; i++)
            {
                os << std::to_string(address.octets[i]);
                if (i < 3)
                    os << ".";
            }
            return os;
        }

        uint8_t GetOctet(std::size_t i) const {
            if (i > 3)
            {
                std::stringstream ss;
                ss << "Invalid octet index: " << std::to_string(i) << std::endl;
                throw ss.str();
            }

            return octets[i];
        }

        bool HasOctet(uint8_t value) const {
            return std::find(octets.cbegin(), octets.cend(), value) != std::end(octets);
        };

        bool operator==(const TAddress& rhs) const {
            return this->octets == rhs.octets;
        }

    private:
        std::array<uint8_t, 4> octets;

        void GetNextByte(const std::string& in, uint8_t& out, std::size_t& start);
    };

}

#endif
