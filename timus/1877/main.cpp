#include <iostream>

bool solve(unsigned short code0, unsigned short code1) {
    return (code0 % 2 == 0 || code1 % 2 == 1);
}

int main() {
    unsigned short code0, code1;
    std::cin >> code0 >> code1;
    auto out = solve(code0, code1);
    if (out) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
    return 0;
}