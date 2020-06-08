#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <numeric>
#include "my_defs.h"

AdjacencyList read_graph_list(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("Bad input file or file does not exist \"" + filename + "\"");
    }
    std::ifstream input{filename};
    AdjacencyList list{};

    int num_vertexes = 0;
    input >> num_vertexes;
    list.resize(num_vertexes);

    for (int i = 0; i < num_vertexes; ++i) {
        for (int j = 0; j < num_vertexes; ++j) {
            int weight= 0;
            input >> weight;
            if (weight != 0) {
                list[i].emplace_back(j, weight);
            }
        }
    }
    return std::move(list);
}

void write_distances(const std::string& filename, const std::vector<int>& distances) {
    std::ofstream output{filename};
    int max_dist = std::numeric_limits<int>::max();
    for (auto dist : distances) {
        output << (dist != max_dist ? std::to_string(dist) : "INF") << ' ';
    }
    output << std::endl;
}

omp_sched_t parse_schedule(const std::string& schedule) {
    if (schedule == "static") {
        return omp_sched_static;
    } else if (schedule == "dynamic") {
        return omp_sched_dynamic;
    } else if (schedule == "guided") {
        return omp_sched_guided;
    } else if (schedule == "auto") {
        return omp_sched_auto;
    }
    throw std::runtime_error("Unknown schedule type");
}

template <typename Runnable, typename TimeMeasure>
struct TimeCounterDecorator {
    TimeCounterDecorator(const Runnable& runnable, int runTimes) : runnable(runnable), run_times(runTimes) {}

    template <typename... Args>
    auto operator()(Args&& ... args) {
        std::vector<uint64_t> durations{};
        using ReturnType = std::invoke_result_t<Runnable, Args...>;
        ReturnType return_value;
        for (int i = 0; i < run_times; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            return_value = runnable(std::forward<Args>(args)...);
            auto end = std::chrono::high_resolution_clock::now();
            durations.push_back(std::chrono::duration_cast<TimeMeasure>(end - start).count());
        }
        run_time = mean(durations);
        return std::move(return_value);
    }

    uint64_t get_run_time() {
        return run_time;
    }

private:
    Runnable runnable;
    int run_times = 1;
    uint64_t run_time = 0;

    template <typename T>
    T mean(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0) / vec.size();
    }
};

template <typename Runnable>
auto decorate_count_microseconds(int run_times, const Runnable& runnable) {
    return TimeCounterDecorator<Runnable, std::chrono::microseconds>(runnable, run_times);
}