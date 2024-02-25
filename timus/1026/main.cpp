#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int main() {
    uint32_t N;
    std::cin >> N;
    std::vector<uint16_t> numbers;
    numbers.reserve(N);

    uint16_t number;
    for (; N > 0; N--) {
        std::cin >> number;
        numbers.push_back(number);
    }

    std::sort(numbers.begin(), numbers.end());

    //bypass '###'
    std::string s;
    std::cin >> s;

    uint16_t K;
    std::cin >> K;
    std::size_t request;
    for (; K > 0; K--) {
        std::cin >> request;
        std::cout << numbers[request - 1] << std::endl;
    }
}
