#include <iostream>
#include <functional>
#include "util.h"
#include "matrix.h"
#include "matrix_multiplication.h"
#include "command_line.h"

using namespace details;

const auto DEFAULT_OUTPUT_FILE = "result.txt";

std::function<void (const Matrix<long>&, const Matrix<long>&, Matrix<long>&)>
get_run_mode(const std::string& mode_name, const Options& options = {}) {
    if (mode_name == "sequential") {
        return matrix_multiplication_sequential;
    }
    if (mode_name == "parallel") {
        auto thread_num = options.get_int_option_or_default("thread_num", -1);
        return [thread_num] (const auto& a, const auto& b, auto& res) {
            matrix_multiplication_parallel(a, b, res, thread_num);
        };
    }
    if (mode_name == "parallel_dynamic") {
        auto thread_num = options.get_int_option_or_default("thread_num", -1);
        auto chunk_size = options.get_int_option_or_default("chunk_size", -1);
        return [thread_num, chunk_size] (const auto& a, const auto& b, auto& res) {
            matrix_multiplication_parallel_dynamic(a, b, res, thread_num, chunk_size);
        };
    }
    if (mode_name == "parallel_guided") {
        auto thread_num = options.get_int_option_or_default("thread_num", -1);
        auto chunk_size = options.get_int_option_or_default("chunk_size", -1);
        return [thread_num, chunk_size] (const auto& a, const auto& b, auto& res) {
            matrix_multiplication_parallel_guided(a, b, res, thread_num, chunk_size);
        };
    }
    // sequential mode as default
    return matrix_multiplication_sequential;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Wrong usage!" << std::endl;
        return 1;
    }
    CommandLineArguments arguments{argc, argv};
    auto named_options = arguments.get_options();

    auto is_orc = named_options.get_bool_option_or_default("is_orc", true);
    auto a = read_matrix_from_file<long>(arguments.at(1));
    auto b = read_matrix_from_file<long>(arguments.at(2), is_orc);
    std::string mode = (argc >= 3) ? arguments.at(3) : "";
    auto partially_applied = get_run_mode(mode, named_options);

    Matrix<long> result{a.height(), b.width()};

    auto runnable = [&a, &b, &result, &partially_applied] () {
        partially_applied(a, b, result);
    };

    auto run_repetitions = named_options.get_int_option_or_default("runs", 1);
    auto milliseconds_counter = TimeCounter<std::chrono::milliseconds>::count_time<decltype(runnable)>;
    std::cout << milliseconds_counter(run_repetitions, runnable) << std::endl;
    write_matrix_to_file(DEFAULT_OUTPUT_FILE, result);

    return 0;
}
