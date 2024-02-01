#include "allocator.hpp"
#include "container.hpp"
#include "factorial.hpp"

#include <map>
#include <iostream>

template <typename T>
void fillMap(T& map) {
    for (auto i = 0; i < 10; i++) {
        map[i] = NAllocator::Factorial(i);
    }
}

template <typename T>
void printMap(const T& map) {
    for (const auto& [key, value] : map) {
        std::cout << key << " " << value << std::endl;
    }
}

template <typename T>
void fillVector(T& vec) {
    for (auto i = 0; i < 10; i++) {
        vec.push(NAllocator::Factorial(i));
    }
}

template <typename T>
void printVector(const T& vec) {
    for (std::size_t i = 0; i < vec.size(); i++) {
        std::cout << i << " " << vec[i] << std::endl;
    }
}

int main() {
    std::cout << std::endl << ">>> std::map<int, int>: " << std::endl;
    std::map<int, int> m1;
    fillMap(m1);
    printMap(m1);

    std::cout << std::endl
              << ">>> std::map<int, int, std::less<int>, NAllocator::TAllocator<std::pair<const int, int>, 10>>: "
              << std::endl;
    std::map<int, int, std::less<int>, NAllocator::THeapAllocator<std::pair<const int, int>, 10>> m2;
    fillMap(m2);
    printMap(m2);

    std::cout << std::endl << ">>> NAllocator::TContainer<int>: " << std::endl;
    NAllocator::TContainer<int> c1;
    fillVector(c1);
    printVector(c1);

    std::cout << std::endl
              << ">>> NAllocator::TContainer<int, NAllocator::TAllocator<std::pair<const int, int>, 10>>: "
              << std::endl;
    NAllocator::TContainer<int> c2;
    fillVector(c2);
    printVector(c2);

    return 0;
}