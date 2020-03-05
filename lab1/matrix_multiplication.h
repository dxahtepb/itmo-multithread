#pragma once
#include "matrix.h"

void matrix_multiplication_sequential(Matrix<int> const& matrix_a, Matrix<int> const& matrix_b, Matrix<int>& result) {
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

void matrix_multiplication_parallel(Matrix<int> const& matrix_a, Matrix<int> const& matrix_b, Matrix<int>& result) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    #pragma omp parallel for
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
