#pragma once

#include <cstddef>
#include <iterator>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

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
            auto lower = std::lower_bound(Items.begin(), Items.end(), TRowItem{.index = ix, .row = nullptr});

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

        std::string Dump(std::size_t rowMin, std::size_t rowMax, std::size_t colMin, std::size_t colMax) const {
            std::stringstream ss;

            auto it = std::lower_bound(Items.begin(), Items.end(), TRowItem{.index = rowMin});
            const auto end = std::upper_bound(Items.begin(), Items.end(), TRowItem{.index = rowMax});
            std::size_t ix = rowMin;

            while (it != end) {
                while (ix < it->index) {
                    ss << DefaultRow.Dump(colMin, colMax) << std::endl;
                    ix++;
                }

                ss << it->row->Dump(colMin, colMax) << std::endl;
                it++;
                ix++;
            }

            return ss.str();
        }

    public:
        struct TIterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            // return custom type to satisfy homework requirements: <rowId, colId, Value>
            using value_type = std::tuple<std::size_t, std::size_t, T>;
            using pointer = T*;
            using reference = T&;

            TIterator(typename std::list<TRowItem>::iterator rowsBegin, typename std::list<TRowItem>::iterator rowsEnd,
                      typename TRow<T, V>::TIterator cellBegin)
                : rowsIt(rowsBegin)
                , rowsEnd(rowsEnd)
                , cellIt(cellBegin){};

            value_type operator*() const {
                // FIXME: is it necessary to make a copy?
                auto rowId = rowsIt->index;
                auto value = *cellIt;
                return std::make_tuple<std::size_t, std::size_t, T>(std::move(rowId), std::move(value.first),
                                                                    std::move(value.second));
            }

            pointer operator->() {
                return cellIt;
            }

            TIterator& operator++() {
                // FIXME: why just `cellIt++` does not compile?
                cellIt = std::next(cellIt);

                if (cellIt == rowsIt->row->end()) {
                    rowsIt++;

                    // switch cell iterator to the next row
                    if (rowsIt != rowsEnd) {
                        cellIt = rowsIt->row->begin();
                    }
                }

                return *this;
            }

            friend bool operator==(const TIterator& a, const TIterator& b) {
                return a.rowsIt == b.rowsIt && a.cellIt == b.cellIt;
            };

            friend bool operator!=(const TIterator& a, const TIterator& b) {
                return a.rowsIt != b.rowsIt || a.cellIt != b.cellIt;
            };

        private:
            typename std::list<TRowItem>::iterator rowsIt;  // iterator over all rows
            typename std::list<TRowItem>::iterator rowsEnd; // end of rows
            typename TRow<T, V>::TIterator cellIt;          // iterator over the cells of a particular row
        };

        TIterator begin() {
            return TIterator(Items.begin(), Items.end(), Items.front().row->begin());
        }

        TIterator end() {
            return TIterator(Items.end(), Items.end(), Items.back().row->end());
        }

        std::size_t LongestRowLength() const {
            int result = 0;

            for (const auto& item : Items) {
                if (item.row->LastIndex() > result) {
                    result = item.row->LastIndex();
                }
            }

            return static_cast<std::size_t>(result);
        }

    private:
        std::list<TRowItem> Items; // rows

        const TRow<T, V> DefaultRow = TRow<T, V>();
    };
} //namespace NMatrix
