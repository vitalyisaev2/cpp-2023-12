#include <iostream>

std::pair<int, int> solve(int n1, int n2) {
    auto total = n1 + n2 - 1;
    auto m1 = total - n1;
    auto m2 = total - n2;
    return std::make_pair(m1, m2);
}

int main() {
    int n1, n2;
    std::cin >> n1 >> n2;
    const auto [m1, m2] = solve(n1, n2);
    std::cout << m1 << " " << m2 << std::endl;
    return 0;
}