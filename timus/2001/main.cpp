#include <iostream>

std::pair<int, int> solve(int a1, int a3, int b1, int b2) {
    return std::make_pair(a1-a3, b1-b2);
}

int main() {
    int a1, b1;
    std::cin >> a1 >> b1;
    int a2, b2;
    std::cin >> a2 >> b2;
    int a3, b3;
    std::cin >> a3 >> b3;
    const auto [m1, m2] = solve(a1, a3, b1, b2);
    std::cout << m1 << " " << m2 << std::endl;
    return 0;
}