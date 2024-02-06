#include <iostream>

int main() {
    unsigned short carsCap, minutes;
    std::cin >> carsCap >> minutes;

    unsigned long carsDebt = 0;
    for (auto i = 0; i < minutes; i++) {
        unsigned short carsNew;
        std::cin >> carsNew;

        auto carsTotal = carsNew + carsDebt;

        if (carsCap >= carsTotal) {
            carsDebt = 0;
        } else {
            carsDebt = carsTotal - carsCap;
        }
    }

    std::cout << carsDebt << std::endl;
}