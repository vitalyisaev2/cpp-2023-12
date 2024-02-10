#include <iostream>

int main() {
    unsigned n, k;
    std::cin >> n >> k;

    if (k > n) {
        std::cout << 2 << std::endl;
        return 0;
    }

    if (2 * n % k == 0) {
        std::cout << 2 * n / k << std::endl;
    } else {
        std::cout << 2 * n / k + 1 << std::endl;
    }
    return 0;
}
