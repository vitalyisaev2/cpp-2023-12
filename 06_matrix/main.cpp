#include <cstddef>
#include <iterator>
#include <list>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>

template <class T, int V>
class TRow {
public:
    using TPtr = std::unique_ptr<TRow<T, V>>;

private:
    struct TCellItem {
        std::size_t index;
        T value = static_cast<T>(V);

        bool operator<(const TCellItem& other) const {
            return index < other.index;
        }
    };

public:
    T operator[](std::size_t ix) const {
        auto pair = std::equal_range(Items.cbegin(), Items.cend(), TCellItem{.index = ix});
        if (pair.first == pair.second) {
            // nothing found, return empty value
            return static_cast<T>(V);
        }

        return pair.first->value;
    }

    T& operator[](std::size_t ix) {
        auto lower = std::lower_bound(Items.begin(), Items.end(), TCellItem{.index = ix});

        // list is empty, add first item
        if (lower == Items.end()) {
            Items.push_back(TCellItem{.index = ix});
            return Items.back().value;
        }

        // exact item found
        if (lower->index == ix) {
            return lower->value;
        }

        // lower->index > ix
        // need to insert new value;
        return Items.insert(lower, TCellItem{.index = ix})->value;
    }

    std::size_t Size() const {
        return Items.size();
    }

    std::size_t LastIndex() const {
        return Items.back().index;
    }

    std::string Dump(std::size_t maxSize) const {
        std::stringstream ss;

        auto it = std::cbegin(Items);
        std::size_t i = 0;

        while (it != std::cend(Items)) {
            while (i < it->index) {
                ss << V;
                ss << "\t";
                i++;
            }

            ss << it->value << "\t";
            it++;
            i++;
        }

        // fill tail with V values if row's too short
        if (Items.size() > 0) {
            i = Items.back().index + 1;
        }
        for (; i <= maxSize; i++) {
            ss << V << "\t";
        }

        return ss.str();
    }

private:
    std::list<TCellItem> Items;
};

template <class T, int V>
class TMatrix {
private:
    struct TRowItem {
        std::size_t index;
        typename TRow<T, V>::TPtr row = nullptr;

        bool operator<(const TRowItem& other) const {
            return index < other.index;
        }
    };

public:
    const TRow<T, V>& operator[](std::size_t ix) const {
        auto pair = std::equal_range(Items.cbegin(), Items.cend(), TRowItem{.index = ix});
        if (pair.first == pair.second) {
            return DefaultRow;
        }

        return *pair.first->row;
    }

    TRow<T, V>& operator[](std::size_t ix) {
        auto lower = std::lower_bound(Items.begin(), Items.end(), TRowItem{.index = ix});

        // list is empty, add first item
        if (lower == Items.end()) {
            Items.push_back(TRowItem{.index = ix, .row = std::make_unique<TRow<T, V>>()});
            return *Items.back().row;
        }

        // exact item found
        if (lower->index == ix) {
            return *lower->row;
        }

        // lower->index > ix
        // need to insert new value;
        return *Items.insert(lower, TRowItem{.index = ix})->row;
    }

    std::size_t Size() const {
        std::size_t sum = 0;

        for (const auto& row : Items) {
            sum += row.size();
        }

        return sum;
    }

    std::string Dump() const {
        std::stringstream ss;

        const auto maxIndex = LongestRowLength();

        auto it = std::cbegin(Items);
        std::size_t ix = 0;

        while (1) {
            while (ix < it->index) {
                ss << DefaultRow.Dump(maxIndex) << std::endl;
                ix++;
            }

            ss << it->row->Dump(maxIndex) << std::endl;
            it++;
            ix++;

            if (it == std::cend(Items)) {
                break;
            }
        }

        return ss.str();
    }

private:
    std::size_t LongestRowLength() const {
        std::size_t result = 0;

        for (const auto& item : Items) {
            if (item.row->LastIndex() > result) {
                result = item.row->LastIndex();
            }
        }

        return result;
    }

    std::list<TRowItem> Items; // rows

    const TRow<T, V> DefaultRow;
};

int main() {
    TMatrix<int, -1> matrix;
    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;
    std::cout << matrix.Dump() << std::endl;
}
