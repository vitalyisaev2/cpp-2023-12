#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>
#include <string>

#include "matrix.hpp"
#include "row.hpp"

TEST(Row, Iterate) {
    NMatrix::TRow<int, -1> row;

    // fill row, actual values are interleaved with default values
    for (std::size_t i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            row[i] = i;
        }
    }

    // drop cell at ix = 4
    row[4] = -1;

    std::vector<NMatrix::TRow<int, -1>::TIterator::value_type> actual;
    std::copy(row.begin(), row.end(), std::back_inserter(actual));

    std::vector<NMatrix::TRow<int, -1>::TIterator::value_type> expected = {
        {0, 0},
        {2, 2},
        {6, 6},
        {8, 8}
    };

    ASSERT_EQ(actual, expected);
}

TEST(Matrix, Dump) {
    NMatrix::TMatrix<int, -1> matrix;

    const std::string expected =
        R"(-1    -1    -1    -1
-1    1    -1    -1
-1    -1    2    -1
-1    -1    -1    3
)";

    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;

    ASSERT_EQ(matrix.Dump(), expected);
}

TEST(Matrix, Size) {
    NMatrix::TMatrix<int, -1> matrix;

    ASSERT_EQ(matrix.Size(), 0);

    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;

    ASSERT_EQ(matrix.Size(), 3);

    matrix[1][1] = -1;
    matrix[2][2] = -1;
    matrix[3][3] = -1;

    ASSERT_EQ(matrix.Size(), 0);
}
