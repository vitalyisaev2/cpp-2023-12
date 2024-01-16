#include <sstream>
#include <gtest/gtest.h>

#include "address.hpp"
#include "addresses.hpp"

using namespace NIPFilter;

TEST(IPFilter, ParseAddress)
{
    auto expected = TAddress(1, 2, 3, 4);
    auto actual = TAddress("1.2.3.4");
    std::cout << actual;
    ASSERT_EQ(expected, actual);
}


TEST(IPFilter, ParseStream)
{
    std::stringstream ss;
    ss << "1.2.3.4\t10.20.30.40\t100.200.300.400" << std::endl;
    ss << "2.3.4.5\t10.20.30.40\t100.200.300.400" << std::endl;
    ss << "3.4.5.6\t10.20.30.40\t100.200.300.400" << std::endl;

    std::vector<TAddress> expected;
    expected.emplace_back(TAddress(1, 2, 3, 4 ));
    expected.emplace_back(TAddress( 2, 3, 4, 5 ));
    expected.emplace_back(TAddress( 3, 4, 5, 6 ));

    auto actual = TAddresses(ss);

    ASSERT_EQ(expected, actual);
}

TEST(IPFilter, Comparison)
{
    ASSERT_LT(TAddress( 1, 1, 1, 1 ), TAddress( 1, 1, 1, 2 ));
    ASSERT_LT(TAddress( 1, 1, 1, 1 ), TAddress( 1, 1, 2, 1 ));
    ASSERT_LT(TAddress( 1, 1, 1, 1 ), TAddress( 1, 2, 1, 1 ));
    ASSERT_LT(TAddress( 1, 1, 1, 1 ), TAddress( 2, 1, 1, 1 ));
}

TEST(IPFilter, AddressesToStream)
{
    std::stringstream in;
    in << "1.2.3.4\t10.20.30.40\t100.200.300.400" << std::endl;
    in << "2.3.4.5\t10.20.30.40\t100.200.300.400" << std::endl;
    in << "3.4.5.6\t10.20.30.40\t100.200.300.400" << std::endl;

    std::stringstream outExpected;
    outExpected << "1.2.3.4" << std::endl;
    outExpected << "2.3.4.5" << std::endl;
    outExpected << "3.4.5.6" << std::endl;

    std::stringstream outActual;

    auto addresses = TAddresses(in);
    outActual << addresses;

    ASSERT_EQ(outExpected.str(), outActual.str());
}