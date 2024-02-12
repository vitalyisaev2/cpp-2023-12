#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <array>

constexpr uint32_t base = 256;

void ConvertNumberToBase256(uint32_t value, std::array<uint32_t, 4>& coefficients, bool good) {
    coefficients = {0, 0, 0, 0};

    std::size_t i;
    if (good) {
        i = 0;
    } else {
        i = 3;
    }

    while (true) {
        auto division = value / base;
        auto residual = value % base;

        coefficients[i] = residual;

        if (good) {
            i++;
        } else {
            i--;
        }

        if (division < base) {
            coefficients[i] = division;
            return;
        }

        value = division;
    }
}

uint32_t ConvertBase256ToNumber(const std::array<uint32_t, 4>& coefficients, bool good) {
    uint32_t value = 0;

    for (std::size_t i = 0; i < coefficients.size(); i++) {
        if (!good) {
            value += coefficients[i] * std::pow(base, i);
        } else {
            value += coefficients[i] * std::pow(base, coefficients.size() - i - 1);
        }
    }

    return value;
}

int main() {
    std::string kind;
    std::cin >> kind;

    bool good;
    if (kind == "GOOD") {
        good = true;
    } else if (kind == "BAD") {
        good = false;
    } else {
        throw kind;
    }

    uint32_t value;
    std::cin >> value;

    std::array<uint32_t, 4> target;
    ConvertNumberToBase256(value, target, !good);

    std::cout << ConvertBase256ToNumber(target, !good) << std::endl;

    return 0;
}
