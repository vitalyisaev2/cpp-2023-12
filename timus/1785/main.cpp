#include <array>
#include <iostream>

int main() {
    std::array<unsigned, 9> borders{4, 9, 19, 49, 99, 249, 499, 999, 2001};
    std::array<std::string, 9> names{"few", "several", "pack", "lots", "horde", "throng", "swarm", "zounds", "legion"};

    unsigned value;
    std::cin >> value;

    for (std::size_t i = 0; i < borders.size(); i++) {
        if (value <= borders[i]) {
            std::cout << names[i] << std::endl;
            return 0;
        }
    }
}
