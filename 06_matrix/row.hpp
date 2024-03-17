#pragma once

#include <cstddef>
#include <iterator>
#include <list>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>
#include <utility>

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

        std::string Dump(std::size_t colMin, std::size_t colMax) const {
            std::stringstream ss;

            auto it = std::lower_bound(Items.begin(), Items.end(), TCellItem{.index = colMin});

            std::size_t i = colMin;

            while (it != std::cend(Items)) {
                while (i < it->index) {
                    ss << V << Delimiter;
                    i++;
                }

                ss << it->value;
                it++;

                if (i != colMax) {
                    ss << Delimiter;
                }

                i++;
            }

            // fill tail with V values if row's too short
            if (Items.size() > 0) {
                i = Items.back().index + 1;
            }
            for (; i <= colMax; i++) {
                ss << V;
                if (i != colMax) {
                    ss << Delimiter;
                }
            }

            return ss.str();
        }

    private:
        const std::string Delimiter = "    ";
        const int EmptyRow = -1;

        std::list<TCellItem> Items;

    public:
        struct TIterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<std::size_t, T>;
            using pointer = T*;
            using reference = T&;

            explicit TIterator(typename std::list<TCellItem>::iterator cellsBegin,
                               typename std::list<TCellItem>::iterator cellsEnd)
                : cellsIt(cellsBegin)
                , cellsEnd(cellsEnd){};

            // return custom type to satisfy homework requirements
            value_type operator*() const {
                // FIXME: is it necessary to make a copy?
                auto ix = cellsIt->index;
                auto value = cellsIt->value;
                return std::make_pair<std::size_t, T>(std::move(ix), std::move(value));
            }

            pointer operator->() {
                return &cellsIt->value;
            }

            TIterator& operator++() {
                // look for a first cell with value other than default
                do {
                    cellsIt++;
                } while (cellsIt != cellsEnd && cellsIt->value == V);

                return *this;
            }

            friend bool operator==(const TIterator& a, const TIterator& b) {
                return a.cellsIt == b.cellsIt;
            };

            friend bool operator!=(const TIterator& a, const TIterator& b) {
                return a.cellsIt != b.cellsIt;
            };

        private:
            typename std::list<TCellItem>::iterator cellsIt;
            typename std::list<TCellItem>::iterator cellsEnd;
        };

        TIterator begin() {
            return TIterator(Items.begin(), Items.end());
        }

        TIterator end() {
            return TIterator(Items.end(), Items.end());
        }
    };

} //namespace NMatrix
