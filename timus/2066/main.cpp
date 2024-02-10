#include <iostream>
#include <array>
#include <algorithm>

int main() {
    std::array<long long, 3> data;
    std::cin >> data[0] >> data[1] >> data[2];
    std::sort(data.begin(), data.end());

    auto result = std::min({
        data[0] - data[1] * data[2],
        data[0] - data[1] - data[2],
    });

    std::cout << result << std::endl;
    return 0;
}
