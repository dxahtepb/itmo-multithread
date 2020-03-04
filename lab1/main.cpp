#include <iostream>
#include "util.hpp"
#include "matrix.hpp"

using namespace details;

void matrix_multiplication(Matrix<int> const& matrix_a, Matrix<int> const& matrix_b, Matrix<int>& result) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    for (size_t i = 0; i < result.height(); ++i) {
        for (size_t j = 0; j < result.width(); ++j) {
            int64_t res = 0;
            for (size_t k = 0; k < matrix_a.width(); ++k) {
                res += matrix_a[i][k] * matrix_b[k][j];
            }
            result[i][j] = res;
        }
    }
}

int main() {
    Matrix a = read_matrix_from_file<int>("../matrices/2x2simple.txt");
    Matrix b = read_matrix_from_file<int>("../matrices/2x2identity.txt");
    Matrix<int> result(a.height(), b.width());

    auto runnable = [&a, &b, &result] () {
        matrix_multiplication(a, b, result);
    };

    std::cout << "Mean time: " << count_time(1, runnable) << std::endl;
    write_matrix_to_file("../matrices/result.txt", result);

    return 0;
}
