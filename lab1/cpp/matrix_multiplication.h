#pragma once
#include "matrix.h"
#include "omp.h"

void matrix_multiplication_sequential(const Matrix<long>& matrix_a, const Matrix<long>& matrix_b,
                                      Matrix<long>& result) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    for (size_t i = 0; i < result.height(); ++i) {
        for (size_t j = 0; j < result.width(); ++j) {
            int64_t res = 0;
            for (size_t k = 0; k < matrix_a.width(); ++k) {
                res += matrix_a.at(i, k) * matrix_b.at(k, j);
            }
            result.at(i, j) = res;
        }
    }
}

void matrix_multiplication_parallel(const Matrix<long>& matrix_a, const Matrix<long>& matrix_b, Matrix<long>& result,
                                    int thread_num) {
    if (matrix_a.width() != matrix_b.height()) {
        throw std::runtime_error("Cannot multiply matrices");
    }

    if (thread_num > 0) {
        omp_set_dynamic(0);
        omp_set_num_threads(thread_num);
    }

    #pragma omp parallel for schedule(static) collapse(2)
    for (size_t i = 0; i < result.height(); ++i) {
        for (size_t j = 0; j < result.width(); ++j) {
            int64_t res = 0;
            for (size_t k = 0; k < matrix_a.width(); ++k) {
                res += matrix_a.at(i, k) * matrix_b.at(k, j);
            }
            result.at(i, j) = res;
        }
    }
}

void matrix_multiplication_parallel_dynamic(const Matrix<long>& matrix_a, const Matrix<long>& matrix_b,
                                            Matrix<long>& result, int thread_num, int chunk_size) {
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

    #pragma omp parallel for schedule(dynamic, chunk_size) collapse(2)
    for (size_t i = 0; i < result.height(); ++i) {
        for (size_t j = 0; j < result.width(); ++j) {
            int64_t res = 0;
            for (size_t k = 0; k < matrix_a.width(); ++k) {
                res += matrix_a.at(i, k) * matrix_b.at(k, j);
            }
            result.at(i, j) = res;
        }
    }
}

void matrix_multiplication_parallel_guided(const Matrix<long>& matrix_a, const Matrix<long>& matrix_b,
                                            Matrix<long>& result, int thread_num, int chunk_size) {
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

    #pragma omp parallel for schedule(guided, chunk_size) collapse(2)
    for (size_t i = 0; i < result.height(); ++i) {
        for (size_t j = 0; j < result.width(); ++j) {
            int64_t res = 0;
            for (size_t k = 0; k < matrix_a.width(); ++k) {
                res += matrix_a.at(i, k) * matrix_b.at(k, j);
            }
            result.at(i, j) = res;
        }
    }
}