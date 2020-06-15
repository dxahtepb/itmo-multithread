#pragma once

#include <vector>
#include <limits>
#include <queue>
#include <omp.h>
#include <algorithm>
#include "my_defs.h"


int min_distance(const std::vector<int>& dist, const std::vector<bool>& used_set) {
    int min_value = std::numeric_limits<int>::max();
    int min_index = 0;

    for (int v = 0; v < dist.size(); ++v) {
        if (!used_set[v] && dist[v] <= min_value) {
            min_value = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

std::vector<int> sequential_dijkstra(const AdjacencyList& matrix, int source) {
    int size = matrix.size();
    std::vector<int> dist(size, std::numeric_limits<int>::max());
    std::vector<bool> used(size);
    dist[source] = 0;
    for (int count = 0; count < size - 1; ++count) {
        int from = min_distance(dist, used);
        if (dist[from] == std::numeric_limits<int>::max()) {
            break;
        }
        used[from] = true;
        for (const auto& edge : matrix[from]) {
            auto to = edge.to;
            auto weight = edge.weight;
            auto relaxed_dist = dist[from] + weight;
            if (!used[to] && relaxed_dist < dist[to]) {
                dist[to] = relaxed_dist;
            }
        }
    }
    return std::move(dist);
}

int min_distance_omp(const std::vector<int>& dist, const std::vector<bool>& used_set, int max_threads) {
    std::vector<std::pair<int, int>> local_minimums(max_threads, {std::numeric_limits<int>::max(), -1});

    #pragma omp parallel
    {
        int local_min = std::numeric_limits<int>::max();
        int local_min_index = 0;
        #pragma omp for
        for (int v = 0; v < dist.size(); ++v) {
            if (!used_set[v] && dist[v] <= local_min) {
                local_min = dist[v];
                local_min_index = v;
            }
        }
        auto this_thread_num = omp_get_thread_num();
        local_minimums[this_thread_num] = {local_min, local_min_index};
    }
    return std::min_element(local_minimums.begin(), local_minimums.end())->second;
}

int min_distance_omp_reduce(const std::vector<int>& dist, const std::vector<bool>& used_set, int max_threads) {
    auto min_value_index_pair = std::make_pair<>(std::numeric_limits<int>::max(), 0);

    #pragma omp declare reduction \
    (min_index_reduction : decltype(min_value_index_pair) : omp_out = std::min(omp_out, omp_in)) \
    initializer(omp_priv = {std::numeric_limits<int>::max(), 0})

    #pragma omp parallel for reduction(min_index_reduction : min_value_index_pair)
    for (int v = 0; v < dist.size(); ++v) {
        if (!used_set[v] && dist[v] <= min_value_index_pair.first) {
            min_value_index_pair = {dist[v], v};
        }
    }
    return min_value_index_pair.second;
}

std::vector<int> dijkstra_omp(const AdjacencyList& matrix, int source,
                              int num_threads, omp_sched_t schedule, int chunk_size) {
    int size = matrix.size();
    std::vector<int> dist(size, std::numeric_limits<int>::max());
    std::vector<bool> used(size);

    if (num_threads != -1) {
        omp_set_dynamic(0);
        omp_set_num_threads(std::min(num_threads, size));
    }
    omp_set_schedule(schedule, chunk_size);
    int max_threads = omp_get_max_threads();

    dist[source] = 0;
    for (int step = 0; step < size - 1; ++step) {
        int from = min_distance_omp_reduce(dist, used, max_threads);
        if (dist[from] == std::numeric_limits<int>::max()) {
            break;
        }
        used[from] = true;
        #pragma omp parallel for
        for (size_t i = 0; i < matrix[from].size(); ++i) {
            const auto& edge = matrix[from][i];
            auto to = edge.to;
            auto weight = edge.weight;
            auto relaxed_dist = dist[from] + weight;
            if (!used[to] && relaxed_dist < dist[to]) {
                dist[to] = relaxed_dist;
            }
        }
    }
    return std::move(dist);
}
