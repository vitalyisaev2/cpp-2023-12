#pragma once

#include <cstddef>
#include <iterator>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "row.hpp"

namespace NMatrix {
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

            for (const auto& item : Items) {
                sum += item.row->Size();
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
            int result = 0;

            for (const auto& item : Items) {
                if (item.row->LastIndex() > result) {
                    result = item.row->LastIndex();
                }
            }

            return static_cast<std::size_t>(result);
        }

        std::list<TRowItem> Items; // rows

        const TRow<T, V> DefaultRow;
    };
} //namespace NMatrix
