#include <cstddef>
#include <list>
#include <algorithm>
#include <iostream>

template <class T, int V>
class TRow {
private:
    struct TItem {
        std::size_t index;
        T value = static_cast<T>(V);

        bool operator<(const TItem& other) const {
            return index < other.index;
        }
    };

public:
    T operator[](std::size_t ix) const {
        auto pair = std::equal_range(items.cbegin(), items.cend(), TItem{.index = ix});
        if (pair.first == pair.second) {
            return static_cast<T>(V);
        }

        return pair.first->value;
    }

    T& operator[](std::size_t ix) {
        auto lower = std::lower_bound(items.begin(), items.end(), TItem{.index = ix});

        // list is empty, add first item
        if (lower == items.end()) {
            items.push_back(TItem{.index = ix});
            return items.back().value;
        }

        // exact item found
        if (lower->index == ix) {
            return lower->value;
        }

        // lower->index > ix
        // need to insert new value;
        return items.insert(lower, TItem{.index = ix})->value;
    }

    std::size_t Size() const {
        return items.size();
    }

private:
    std::list<TItem> items;
};

int main() {
    TRow<int, -1> row;

    for (std::size_t i = 10; i > 0; i--) {
        if (i % 2 == 0) {
            row[i] = i * i;
        }
    }

    for (std::size_t i = 0; i < 10; i++) {
        std::cout << row[i] << std::endl;
    }
}
