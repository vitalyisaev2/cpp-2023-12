#include <cstddef>
#include <iostream>
#include <string_view>
#include <set>

std::size_t longString(const std::string& s) {
    std::set<std::size_t> numbers;

    for (std::size_t i = 0; i < s.size(); i++) {
        char ch = s[i];
        if (ch == '0') {
            continue;
        }

        auto limit = std::min(std::size_t(i+6), s.size());
        for (std::size_t j = i + 1; j <= limit; j++) {
            std::string_view slice{s.c_str() + i, j - i};
            std::size_t number = std::stoull(std::string(slice));
            numbers.emplace(number);
        }
    }

    auto it = numbers.cbegin();
    std::size_t prev = *it;

    if (prev != 1) {
        return 1;
    }

    if (prev == 1 && numbers.size() == 1) {
        return 2;
    }

    it++;

    for (; it != numbers.cend(); it++) {
        auto curr = *it;
        if (curr != prev + 1) {
            return prev + 1;
        }

        prev = curr;
    }

    return *numbers.cbegin() - 1;
}

int main() {
    std::string s;
    std::cin >> s;

    std::cout << longString(s) << std::endl;
}
