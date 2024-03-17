#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>
#include <string>

#include "matrix.hpp"
#include "row.hpp"

TEST(Row, Dump) {
    NMatrix::TRow<int, -1> row;

    // fill row, actual values are interleaved with default values
    for (std::size_t i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            row[i] = i;
        }
    }

    std::string expected;
    expected = "0    -1    2    -1    4    -1    6    -1    8    -1";
    ASSERT_EQ(row.Dump(0, 9), expected);

    expected = "-1    2    -1    4    -1    6    -1    8";
    ASSERT_EQ(row.Dump(1, 8), expected);
}

TEST(Matrix, Dump) {
    NMatrix::TMatrix<int, -1> matrix;

    matrix[0][0] = 0;
    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;

    std::string expected;
    expected =
        R"(0    -1    -1    -1
-1    1    -1    -1
-1    -1    2    -1
-1    -1    -1    3
)";

    ASSERT_EQ(matrix.Dump(0, 3, 0, 3), expected);

    expected =
    R"(1    -1
-1    2
)";
    ASSERT_EQ(matrix.Dump(1, 2, 1, 2), expected);
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

TEST(Row, Iterator) {
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

TEST(Matrix, Iterator) {
    NMatrix::TMatrix<int, -1> matrix;

    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;

    std::vector<NMatrix::TMatrix<int, -1>::TIterator::value_type> actual;
    std::copy(matrix.begin(), matrix.end(), std::back_inserter(actual));

    std::vector<NMatrix::TMatrix<int, -1>::TIterator::value_type> expected = {
        {1, 1, 1},
        {2, 2, 2},
        {3, 3, 3},
    };

    ASSERT_EQ(actual, expected);
}
