#pragma once
#include "matrix.h"
#include "omp.h"

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

void matrix_multiplication_parallel(Matrix<int> const& matrix_a, Matrix<int> const& matrix_b, Matrix<int>& result,
                                    int thread_num) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    if (thread_num > 0) {
        omp_set_dynamic(0);
        omp_set_num_threads(thread_num);
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

void matrix_multiplication_parallel_dynamic(
        Matrix<int> const& matrix_a,
        Matrix<int> const& matrix_b,
        Matrix<int>& result,
        int thread_num,
        int chunk_size) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    if (thread_num > 0) {
        omp_set_dynamic(0);
        omp_set_num_threads(thread_num);
    }

    if (chunk_size < 0) {
        chunk_size = static_cast<int>(result.height()) / omp_get_max_threads();
    }

    #pragma omp parallel for schedule(dynamic, chunk_size)
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
