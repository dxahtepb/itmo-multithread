#pragma once

#include <vector>
#include <unordered_map>

struct Edge {
    int to;
    int weight;

    Edge(int to, int weight) : to(to), weight(weight) {}
    Edge(Edge&&) = default;
    Edge(const Edge&) = default;
    Edge& operator=(const Edge&) = default;
    Edge& operator=(Edge&&) = default;
};
using AdjacencyList = std::vector<std::vector<Edge>>;
using DijkstraSSP = std::function<std::vector<int> (const AdjacencyList&, int)>;
