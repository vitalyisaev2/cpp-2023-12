#include <gtest/gtest.h>
#include <string>

#include "matrix.hpp"

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
