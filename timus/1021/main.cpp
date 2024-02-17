#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

std::vector<int16_t> fillNumbers() {
    uint16_t n;
    std::cin >> n;
    std::vector<int16_t> result;
    result.reserve(n);

    int16_t value;
    for (std::size_t i = 0; i < n; i++) {
        std::cin >> value;
        result.push_back(value);
    }

    return result;
}

int main() {
    auto v1 = fillNumbers();
    auto v2 = fillNumbers();

    for (const auto x : v2) {
        auto supp = 10000 - x;
        if (std::binary_search(v1.begin(), v1.end(), supp)) {
            std::cout << "YES" << std::endl;
            return 0;
        }
    }

    std::cout << "NO" << std::endl;
    return 0;
}
