#include <iostream>
#include <vector>
#include <list>

#include "print_ip.hpp"

int main() {
    std::cout << NPrintIP::PrintIP(int8_t{-1}) << std::endl;
    std::cout << NPrintIP::PrintIP(int16_t{0}) << std::endl;
    std::cout << NPrintIP::PrintIP(int32_t{2130706433}) << std::endl;
    std::cout << NPrintIP::PrintIP(int64_t{8875824491850138409}) << std::endl;
    std::cout << NPrintIP::PrintIP(std::string{"Hello, World!"}) << std::endl;
    std::cout << NPrintIP::PrintIP(std::vector<int>{100, 200, 300, 400}) << std::endl;
    std::cout << NPrintIP::PrintIP(std::list<short>{400, 300, 200, 100}) << std::endl;
    std::cout << NPrintIP::PrintIP(std::make_tuple(123, 456, 789, 0)) << std::endl;
}
