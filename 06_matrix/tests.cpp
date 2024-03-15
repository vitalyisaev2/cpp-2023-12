#include <gtest/gtest.h>
#include <string>

#include "matrix.hpp"
#include "row.hpp"

TEST(Row, Iterate) {
    NMatrix::TRow<int, -1> row;
    for (std::size_t i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            row[i] = i;
        }
    }

    // drop cell at ix = 4
    row[4] = -1;

    for (const auto& item : row) {
        std::size_t index;
        int value;
        std::tie(index, value) = item;
        std::cout << index << " " << value << std::endl;
    }
}

TEST(Matrix, Dump) {
    NMatrix::TMatrix<int, -1> matrix;

    const std::string expectedOutput =
        R"(-1    -1    -1    -1
-1    1    -1    -1
-1    -1    2    -1
-1    -1    -1    3
)";

    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;

    ASSERT_EQ(matrix.Dump(), expectedOutput);
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
