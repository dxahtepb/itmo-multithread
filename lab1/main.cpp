#include <iostream>
#include <functional>
#include <unordered_map>
#include <any>
#include "util.h"
#include "matrix.h"
#include "matrix_multiplication.h"

using namespace details;

auto get_run_mode(const std::string& mode_name, const std::unordered_map<std::string, std::any>& params = {}) {
    using multiplication_func = std::function<void (Matrix<int> const&, Matrix<int> const&, Matrix<int>&)>;
    static std::unordered_map<std::string, multiplication_func> modes {
        {"sequential", matrix_multiplication_sequential},
        {"parallel", matrix_multiplication_parallel}
    };
    return modes.at(mode_name);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cout << "Wrong usage!" << std::endl;
        return 1;
    }
    auto a = read_matrix_from_file<int>(argv[1]);
    auto b = read_matrix_from_file<int>(argv[2]);

    Matrix<int> result(a.height(), b.width());

    std::string mode = argv[4];
    auto partially_applied = get_run_mode(mode);
    auto runnable = [&a, &b, &result, &partially_applied] () {
        partially_applied(a, b, result);
    };

    auto milliseconds_counter = TimeCounter<std::chrono::milliseconds>::count_time<decltype(runnable)>;
    std::cout << "Mean time: " << milliseconds_counter(1, runnable) << std::endl;
    write_matrix_to_file(argv[3], result);

    return 0;
}
