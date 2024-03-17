#include "matrix.hpp"

int main() {
    // Fill matrix
    NMatrix::TMatrix<int, 0> matrix;

    const std::size_t size = 10;

    // main diagonal
    for (std::size_t i = 0; i < size; i++) {
        matrix[i][i] = i;
    }

    // secondary diagonal
    for (std::size_t i = 0; i < size; i++) {
        matrix[size - i - 1][i] = i;
    }

    std::cout << std::endl << "Iterate through occupied cells: " << std::endl;
    for (const auto& record : matrix) {
        std::size_t rowId;
        std::size_t colId;
        std::size_t value;
        std::tie(rowId, colId, value) = record;
        std::cout << "rowId: " << rowId << ", colId: " << colId << ", value: " << value << std::endl;
    }

    std::cout << std::endl << "Total occupied cells: " << std::endl;
    std::cout << matrix.Size() << std::endl;

    std::cout << std::endl << "Matrix Dump [1:8][1:8]: " << std::endl;
    std::cout << matrix.Dump(1, 8, 1, 8) << std::endl;
}
