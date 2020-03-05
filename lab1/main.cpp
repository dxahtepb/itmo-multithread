#include <iostream>
#include <functional>
#include <unordered_map>
#include <any>
#include "util.h"
#include "matrix.h"
#include "matrix_multiplication.h"

using namespace details;

const auto DEFAULT_OUTPUT_FILE = "result.txt";

std::function<void (Matrix<int> const&, Matrix<int> const&, Matrix<int>&)>
get_run_mode(const std::string& mode_name, const std::unordered_map<std::string, std::any>& params = {}) {
    if (mode_name == "sequential") {
        return matrix_multiplication_sequential;
    }
    if (mode_name == "parallel") {
        auto thread_num = get_param_or_default(params, "thread_num", -1);
        return [thread_num] (Matrix<int> const& a, Matrix<int> const& b, Matrix<int>& res) {
            matrix_multiplication_parallel(a, b, res, thread_num);
        };
    }
    if (mode_name == "parallel_dynamic") {
        auto thread_num = get_param_or_default(params, "thread_num", -1);
        auto chunk_size = get_param_or_default(params, "chunk_size", -1);
        return [thread_num, chunk_size] (Matrix<int> const& a, Matrix<int> const& b, Matrix<int>& res) {
            matrix_multiplication_parallel_dynamic(a, b, res, thread_num, chunk_size);
        };
    }
    // sequential mode as default
    return matrix_multiplication_sequential;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Wrong usage!" << std::endl;
        return 1;
    }
    auto a = read_matrix_from_file<int>(argv[1]);
    auto b = read_matrix_from_file<int>(argv[2]);

    Matrix<int> result(a.height(), b.width());

    std::string mode = argv[3];
    auto partially_applied = get_run_mode(mode);
    auto runnable = [&a, &b, &result, &partially_applied] () {
        partially_applied(a, b, result);
    };

    auto milliseconds_counter = TimeCounter<std::chrono::milliseconds>::count_time<decltype(runnable)>;
    std::cout << "Mean time: " << milliseconds_counter(1, runnable) << std::endl;
    write_matrix_to_file(DEFAULT_OUTPUT_FILE, result);

    return 0;
}
