#pragma once

#include <cstddef>
#include <iterator>
#include <list>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>

namespace NMatrix {

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
            std::size_t sum = 0;

            for (const auto& item : Items) {
                if (item.value != V) {
                    sum++;
                }
            }

            return sum;
        }

        int LastIndex() const {
            // iterate backwards over the cells and find first non-default value
            auto it = Items.crbegin();
            while (it != Items.crend()) {
                if (it->value != V) {
                    return it->index;
                }
                it++;
            }

            return EmptyRow;
        }

        std::string Dump(std::size_t maxSize) const {
            std::stringstream ss;

            auto it = std::cbegin(Items);
            std::size_t i = 0;

            while (it != std::cend(Items)) {
                while (i < it->index) {
                    ss << V << Delimiter;
                    i++;
                }

                ss << it->value;
                it++;

                if (i != maxSize) {
                    ss << Delimiter;
                }

                i++;
            }

            // fill tail with V values if row's too short
            if (Items.size() > 0) {
                i = Items.back().index + 1;
            }
            for (; i <= maxSize; i++) {
                ss << V;
                if (i != maxSize) {
                    ss << Delimiter;
                }
            }

            return ss.str();
        }

    private:
        const std::string Delimiter = "    ";
        const int EmptyRow = -1;

        std::list<TCellItem> Items;
    };

} //namespace NMatrix
