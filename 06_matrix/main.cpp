#include "matrix.hpp"

int main() {
    NMatrix::TMatrix<int, -1> matrix;
    matrix[1][1] = 1;
    matrix[2][2] = 2;
    matrix[3][3] = 3;
    std::cout << matrix.Dump() << std::endl;
    std::cout << matrix.Size() << std::endl;
}
