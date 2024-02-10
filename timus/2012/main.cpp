#include <iostream>

int main() {
    constexpr unsigned short rest = 4 * 60 / 45;
    unsigned short first;
    std::cin >> first;

    if (first + rest >= 12) {
        std::cout << "YES" << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }
}
