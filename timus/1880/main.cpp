#include <iostream>
#include <unordered_set>
#include <array>

int main() {
    std::array<std::unordered_set<unsigned long long>, 3> storages;

    for (std::size_t i = 0; i < 3; i++) {
        std::size_t n;
        std::cin >> n;

        for (std::size_t j = 0; j < n; j++) {
            unsigned long long number;
            std::cin >> number;
            storages[i].emplace(number);
        }
    }

    std::size_t matched = 0;
    for (const auto number : storages[0]) {
        if (storages[1].contains(number) && storages[2].contains(number)) {
            matched++;
        }
    }

    std::cout << matched << std::endl;

    return 0;
}
