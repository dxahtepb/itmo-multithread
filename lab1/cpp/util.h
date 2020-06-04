#pragma once
#include <vector>
#include <numeric>
#include <chrono>
#include <fstream>
#include <filesystem>
#include "matrix.h"

namespace details {
    template <typename T>
    T mean(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0) / vec.size();
    }

    template <typename TimeMeasure = std::chrono::milliseconds>
    struct TimeCounter {
        template <typename Runnable>
        static auto count_time(int run_times, const Runnable& runnable) {
            std::vector<uint64_t> durations{};
            for (int i = 0; i < run_times; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                runnable();
                auto end = std::chrono::high_resolution_clock::now();
                durations.push_back(std::chrono::duration_cast<TimeMeasure>(end - start).count());
            }
            return details::mean(durations);
        }
    };

    template <typename Runnable>
    static auto count_time(int run_times, const Runnable& runnable) {
        return TimeCounter<>::count_time(run_times, runnable);
    }

    template <typename T>
    void write_matrix_to_file(const std::string& file_name, const Matrix<T>& matrix) {
        std::ofstream output_stream{file_name};
        output_stream << matrix;
    }

    template <typename T>
    Matrix<T> read_matrix_from_file(const std::string& file_name, bool is_orc = false) {
        if (!std::filesystem::exists(file_name)) {
            throw std::runtime_error("Bad input file or file does not exist \"" + file_name + "\"");
        }
        std::ifstream input_stream{file_name};
        size_t height = 0;
        size_t width = 0;
        input_stream >> height >> width;
        Matrix<T> matrix{height, width, is_orc};
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                input_stream >> matrix.at(i, j);
            }
        }
        return matrix;
    }
}
