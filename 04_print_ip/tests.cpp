#include <gtest/gtest.h>

#include "print_ip.hpp"

#include <list>
#include <tuple>

TEST(PrintIP, Integers) {
    ASSERT_EQ(NPrintIP::PrintIP(int8_t{-1}), "255");
    ASSERT_EQ(NPrintIP::PrintIP(int16_t{0}), "0.0");
    ASSERT_EQ(NPrintIP::PrintIP(int32_t{2130706433}), "127.0.0.1");
    ASSERT_EQ(NPrintIP::PrintIP(int64_t{8875824491850138409}), "123.45.67.89.101.112.131.41");
}

TEST(PrintIP, String) {
    ASSERT_EQ(NPrintIP::PrintIP(std::string("Hello, World!")), std::string("Hello, World!"));
}

TEST(PrintIP, Vector) {
    std::vector<int> src{100, 200, 300, 400};
    std::string expected = "100.200.300.400";
    ASSERT_EQ(NPrintIP::PrintIP(src), expected);
}

TEST(PrintIP, List) {
    std::list<short> src{400, 300, 200, 100};
    std::string expected = "400.300.200.100";
    ASSERT_EQ(NPrintIP::PrintIP(src), expected);
}

TEST(PrintIP, Tuple) {
    auto src = std::make_tuple(123, 456, 789, 0);
    std::string expected = "123.456.789.0";
    ASSERT_EQ(NPrintIP::PrintIP(src), expected);
}
