#pragma once

#include <vector>
#include <functional>
#include <cstring>
#include <cmath>
#include <random>

using PivotFinder = std::function<int (const std::vector<int>&)>;

namespace sort_details {
    int sampling_median(const std::vector<int>& vec);
    void gather_data(std::vector<int>& full_vec, const std::vector<int>& batch, int world_size,
                     int rank, MPI_Comm comm);
}

template <typename Comparator = std::less<int>>
void mpi_parallel_sort(std::vector<int>& full_vec, std::vector<int>& batch, int world_size, int world_rank,
                       Comparator comparator = Comparator{},
                       const PivotFinder& get_pivot = sort_details::sampling_median) {
    if (!is_power_of_two(world_size) && world_size != 1) {
        throw std::runtime_error("World size must be power of two");
    }

    MPI_Comm my_group_mpi_comm = MPI_COMM_WORLD;

    for (int local_size = world_size; local_size != 1; local_size /= 2) {
        int my_local_rank = 0;
        MPI_Comm_rank(my_group_mpi_comm, &my_local_rank);

        int pivot = 0;

        std::vector<int> local_full_vec{};
        sort_details::gather_data(local_full_vec, batch, local_size, my_local_rank, my_group_mpi_comm);
        if (my_local_rank == 0) {
            pivot = get_pivot(local_full_vec);
        }
        MPI_Bcast(&pivot, 1, MPI_INT, 0, my_group_mpi_comm);

        auto middle = std::partition(batch.begin(), batch.end(),
                [pivot, &comparator] (const auto& x) { return comparator(x, pivot); });
        int low_size = std::distance(batch.begin(), middle);
        int high_size = std::distance(middle, batch.end());

        int is_in_low_group = my_local_rank < local_size / 2;
        int partner = my_local_rank + (is_in_low_group ? local_size / 2 : -local_size / 2);

        std::vector<int> new_batch{};
        if (is_in_low_group) {
            int partners_low_size = 0;
            MPI_Sendrecv(&high_size, 1, MPI_INT, partner, 0,
                         &partners_low_size, 1, MPI_INT, partner, 0,
                         my_group_mpi_comm, MPI_STATUS_IGNORE);

            new_batch.resize(partners_low_size + low_size);
            std::copy(batch.begin(), middle, std::next(new_batch.begin(), partners_low_size));
            MPI_Sendrecv(&(*middle), high_size, MPI_INT, partner, 0,
                         new_batch.data(), partners_low_size, MPI_INT, partner, 0,
                         my_group_mpi_comm, MPI_STATUS_IGNORE);
        } else {
            int partners_high_size = 0;
            MPI_Sendrecv(&low_size, 1, MPI_INT, partner, 0,
                         &partners_high_size, 1, MPI_INT, partner, 0,
                         my_group_mpi_comm, MPI_STATUS_IGNORE);

            new_batch.resize(partners_high_size + high_size);
            std::copy(middle, batch.end(), std::next(new_batch.begin(), partners_high_size));
            MPI_Sendrecv(batch.data(), low_size, MPI_INT, partner, 0,
                         new_batch.data(), partners_high_size, MPI_INT, partner, 0,
                         my_group_mpi_comm, MPI_STATUS_IGNORE);
        }
        batch = std::move(new_batch);

        int color = is_in_low_group ? 0 : 1;
        MPI_Comm new_comm{};
        MPI_Comm_split(my_group_mpi_comm, color, 0, &new_comm);
        my_group_mpi_comm = new_comm;
    }

    std::sort(batch.begin(), batch.end(), comparator);
    sort_details::gather_data(full_vec, batch, world_size, world_rank, MPI_COMM_WORLD);
}

namespace sort_details {
    void gather_data(std::vector<int>& full_vec, const std::vector<int>& batch, int world_size,
                     int rank, MPI_Comm comm) {
        std::vector<int> batch_sizes(world_size);
        std::vector<int> offsets(world_size);
        int batch_size = batch.size();
        MPI_Gather(&batch_size, 1, MPI_INT, batch_sizes.data(), 1, MPI_INT,0, comm);
        for (int i = 1; i < batch_sizes.size(); ++i) {
            offsets[i] = offsets[i - 1] + batch_sizes[i - 1];
        }
        if (rank == 0) {
            full_vec.resize(std::accumulate(batch_sizes.begin(), batch_sizes.end(), 0));
        }
        MPI_Gatherv(batch.data(), batch.size(), MPI_INT,
                    full_vec.data(), batch_sizes.data(), offsets.data(), MPI_INT,
                    0, comm);
    }

    int sampling_median(const std::vector<int>& vec) {
        std::mt19937 mt{std::random_device{}()};
        std::uniform_int_distribution<uint32_t> dist(0, vec.size() - 1);
        std::vector<int> random_samples{};
        int n_samples = 100;
        random_samples.reserve(n_samples + 3);
        for (int i = 0; i < n_samples; ++i) {
            random_samples.push_back(vec.at(dist(mt)));
        }
        std::sort(random_samples.begin(), random_samples.end());
        return random_samples.at(random_samples.size() / 2);
    }
}

template <typename Comparator = std::less<int>>
void mpi_parallel_sort_merging(std::vector<int>& full_vec, std::vector<int>& batch, int world_size, int world_rank,
        Comparator comparator = std::less<int>{}) {
    std::sort(batch.begin(), batch.end(), comparator);

    for (int step = 1; step < world_size; step = step * 2) {
        if (world_rank % (2 * step) == 0) {
            int partner = world_rank + step;
            if (partner < world_size) {
                int partner_size = 0;
                int my_size = batch.size();
                MPI_Recv(&partner_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                batch.resize(my_size + partner_size);
                MPI_Recv(batch.data() + my_size, partner_size, MPI_INT, partner,
                        0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::inplace_merge(batch.begin(), std::next(batch.begin(), my_size), batch.end());
            }
        } else {
            int partner = world_rank - step;
            int my_size = batch.size();
            MPI_Send(&my_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Send(batch.data(), my_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break;
        }
    }

    if (world_rank == 0) {
        full_vec = std::move(batch);
    }
}