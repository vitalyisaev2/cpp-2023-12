#include <cstddef>
#include <iostream>
#include <string_view>

static bool ends_with(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main() {
    unsigned n;
    std::cin >> n;

    unsigned guests = 2; // married

    std::string data;
    for (std::size_t i = 0; i < n; i++) {
        std::cin >> data;

        if (ends_with(data, "+one")) {
            guests += 2;
        } else {
            guests += 1;
        }
    }

    if (guests == 13) {
        guests++;
    }

    std::cout << 100 * guests << std::endl;

    return 0;
}
