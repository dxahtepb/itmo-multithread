#include <iostream>
#include <functional>
#include <unordered_map>
#include <any>
#include "util.h"
#include "matrix.h"
#include "matrix_multiplication.h"

using namespace details;

constexpr auto DEFAULT_RUN_MODE = "sequential";

auto get_run_mode(const std::string& mode_name, const std::unordered_map<std::string, std::any>& params = {}) {
    using multiplication_func = std::function<void (Matrix<int> const&, Matrix<int> const&, Matrix<int>&)>;
    std::unordered_map<std::string, multiplication_func> modes {
        {"sequential", matrix_multiplication_sequential}
    };
    return modes.at(mode_name);
}

int main(int argc, char* argv[]) {
    Matrix a = read_matrix_from_file<int>("../matrices/2048x2048.txt");
    Matrix b = read_matrix_from_file<int>("../matrices/2048x2048identity.txt");
    Matrix<int> result(a.height(), b.width());

    auto partially_applied = get_run_mode(DEFAULT_RUN_MODE);
    auto runnable = [&a, &b, &result, &partially_applied] () {
        partially_applied(a, b, result);
    };

    using milliseconds_counter = TimeCounter<std::chrono::milliseconds>;
    std::cout << "Mean time: " << milliseconds_counter::count_time(1, runnable) << std::endl;
    write_matrix_to_file("../matrices/result.txt", result);

    return 0;
}
