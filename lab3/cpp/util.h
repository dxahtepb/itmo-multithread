#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <experimental/iterator>

template <typename T>
std::vector<T> read_vec(const std::string& file_name) {
    if (!std::filesystem::exists(file_name)) {
        throw std::runtime_error("Bad input file or file does not exist \"" + file_name + "\"");
    }
    std::ifstream input_stream{file_name};
    std::vector<T> vec{};
    int foo = 0;
    while (input_stream >> foo) {
        vec.push_back(foo);
    }
    return vec;
}

template <typename T>
void write_vec(const std::string& file_name, const std::vector<T>& vec) {
    std::ofstream output_stream{file_name};
    std::copy(vec.begin(), vec.end(),
            std::experimental::make_ostream_joiner(output_stream, " "));
}

bool is_power_of_two(int x) {
    auto xx = static_cast<unsigned int>(x);
    return (x > 0) && ((xx & (xx - 1)) == 0);
}
