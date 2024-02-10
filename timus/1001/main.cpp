#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>

void print_number(unsigned long long src) {
    auto value = sqrtl(src);
    std::cout << std::fixed << std::setprecision(4) << value << std::endl;
}

int main() {
    std::vector<unsigned long long> numbers;

    unsigned long long number;
    while(std::cin >> number) {
        numbers.push_back(number);
    }

    std::for_each(numbers.rbegin(), numbers.rend(), print_number);

    return 0;
}
