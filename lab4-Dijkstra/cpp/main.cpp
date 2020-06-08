#include <iostream>
#include "command_line.h"
#include "dijkstra.h"
#include "util.h"

DijkstraSSP get_run_mode(const std::string& run_mode, const Options& options) {
    if (run_mode == "sequential") {
        return sequential_dijkstra;
    } else if (run_mode == "parallel") {
        auto num_threads = options.get_int_option_or_default("num_threads", -1);
        auto schedule = parse_schedule(options.get_option_or_default("schedule", "static"));
        auto chunk_size = options.get_int_option_or_default("chunk_size", -1);
        return [num_threads, schedule, chunk_size] (const AdjacencyList& matrix, int source) {
            return dijkstra_omp(matrix, source, num_threads, schedule, chunk_size);
        };
    } else {
        throw std::runtime_error("Unknown run mode: " + run_mode);
    }
}

int main(int argc, char* argv[]) {
    CommandLineArguments arguments{argc, argv};
    auto named_options = arguments.get_options();

    auto input_path = arguments.at(1);
    auto starting_point = std::stoi(arguments.at(2));
    auto output_path = arguments.at(3);
    auto run_mode = named_options.get_option_or_default("run_mode", "sequential");
    auto run_times = named_options.get_int_option_or_default("run_times", 1);

    auto graph = read_graph_list(input_path);
    auto runner = decorate_count_microseconds(run_times, get_run_mode(run_mode, named_options));
    auto distances = runner(graph, starting_point);
    std::cout << runner.get_run_time() << std::endl;
    write_distances(output_path, distances);

    return 0;
}