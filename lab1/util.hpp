#pragma once
#include <vector>
#include <numeric>
#include <chrono>
#include <fstream>
#include <experimental/iterator>
#include "matrix.hpp"

namespace details {
    template <typename T>
    T mean(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0) / vec.size();
    }

    template <typename Runnable>
    auto count_time(int run_times, const Runnable& runnable) {
        std::vector<uint64_t> durations{};
        for (int i = 0; i < run_times; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            runnable();
            auto end = std::chrono::high_resolution_clock::now();
            durations.push_back((end - start).count());
        }
        return details::mean(durations);
    }

    template <typename T>
    void write_matrix_to_file(const std::string& file_name, const Matrix<T>& matrix) {
        std::ofstream output_stream{file_name};
        output_stream << matrix;
    }

    template <typename T>
    Matrix<T> read_matrix_from_file(const std::string& file_name) {
        std::ifstream input_stream{file_name};
        size_t height = 0;
        size_t width = 0;
        input_stream >> height >> width;
        Matrix<T> matrix{height, width};
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < height; ++j) {
                input_stream >> matrix[i][j];
            }
        }
        return matrix;
    }
}
