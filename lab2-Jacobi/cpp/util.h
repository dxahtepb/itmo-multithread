#pragma once
#include <cstddef>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <numeric>
#include <experimental/iterator>


int read_matrix(const std::string& filename, std::vector<double>& matrix, std::vector<double>& b) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error{"Bad input file or file does not exist \"" + filename + "\""};
    }
    std::ifstream input_stream{filename};

    int height = 0;
    int width = 0;
    input_stream >> height >> width;
    if (height + 1 != width) {
        throw std::runtime_error("M + 1 != N!");
    }

    matrix.reserve(height * height);
    b.reserve(height);
    for (auto i = 0; i < height; ++i) {
        double foo = 0;
        for (auto j = 0; j < height; ++j) {
             input_stream >> foo;
             matrix.push_back(foo);
        }
        input_stream >> foo;
        b.push_back(foo);
    }

    return height;
}

int read_initial_approximation(const std::string& filename, std::vector<double>& x_initial) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error{"Bad input file or file does not exist \"" + filename + "\""};
    }
    std::ifstream input_stream{filename};

    int size = 0;
    input_stream >> size;

    x_initial.reserve(size);
    for (auto i = 0; i < size; ++i) {
        double foo = 0;
        input_stream >> foo;
        x_initial.push_back(foo);
    }

    return size;
}

void write_vector(const std::string& filename, const std::vector<double>& vec) {
    std::ofstream output_stream{filename};
    output_stream << vec.size() << std::endl;
    std::copy(vec.begin(), vec.end(),
            std::experimental::make_ostream_joiner(output_stream, "\n"));
}
