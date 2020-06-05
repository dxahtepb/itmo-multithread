#pragma once
#include <cmath>
#include <numeric>
#include <experimental/iterator>
#include "LinearSystem.h"

bool is_strictly_diagonal_dominant(const std::vector<double>& matrix, int size) {
    for (auto i = 0; i < size; ++i) {
        auto diagonal_element = fabs(matrix[i * size + i]);
        double row_sum = 0.0;
        for (auto j = 0; j < size; ++j) {
            if (i == j) {
                continue;
            }
            row_sum += fabs(matrix[i * size + j]);
        }
        if (diagonal_element == 0 || diagonal_element <= row_sum) {
            return false;
        }
    }
    return true;
}

bool check_convergence(const std::vector<double>& previous_x, const std::vector<double>& new_x,
        size_t size, double eps) {
    double max_absolute_diff = -1.0;
    for (size_t i = 0; i < size; ++i) {
        double curr_diff = fabs(new_x[i] - previous_x[i]);
        if (max_absolute_diff < curr_diff) {
            max_absolute_diff = curr_diff;
        }
    }
    return max_absolute_diff < eps;
}

std::tuple<bool, std::vector<double>>
jacobi_method_mpi(const LinearSystem& linear_system, int max_iterations, double eps, int world_rank,
        const std::vector<int>& displs, const std::vector<int>& revcounts) {
    int offset = displs.at(world_rank);
    int batch_size = revcounts.at(world_rank);
    int size = linear_system.size;
    std::vector<double> prev_x(linear_system.size);
    std::vector<double> new_x(linear_system.x_initial.begin(), linear_system.x_initial.end());

    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        std::copy(new_x.begin(), new_x.end(), prev_x.begin());
        for (int i = offset; i < offset + batch_size; ++i) {
            auto diagonal_element = linear_system.matrix[i * size + i];
            double acc = 0.0;
            for (int j = 0; j < size; ++j) {
                if (i == j) {
                    continue;
                }
                acc += linear_system.matrix[i * size + j] * prev_x[j];
            }
            new_x[i] = (linear_system.b[i] - acc) / diagonal_element;
        }

        MPI_Allgatherv(new_x.data() + offset, batch_size, MPI_DOUBLE,
                new_x.data(), revcounts.data(), displs.data(), MPI_DOUBLE, MPI_COMM_WORLD);

        if (check_convergence(prev_x, new_x, size, eps)) {
            break;
        }
    }

    return {check_convergence(prev_x, new_x, size, eps), new_x};
}