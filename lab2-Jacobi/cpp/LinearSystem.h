#pragma once

struct LinearSystem {
    int size = 0;
    std::vector<double> matrix;
    std::vector<double> b;
    std::vector<double> x_initial;
    std::vector<double> tmp_matrix;

    LinearSystem(const LinearSystem& other) = delete;
    LinearSystem& operator=(const LinearSystem& other) = delete;

    LinearSystem(LinearSystem&& other) = default;
    LinearSystem& operator=(LinearSystem&& other) = default;
};
