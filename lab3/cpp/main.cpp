#include <iostream>
#include <mpi.h>
#include <chrono>
#include <thread>
#include "command_line.h"
#include "util.h"
#include "sorting.h"

std::tuple<int, int> mpi_init(int* argc, char*** argv) {
    MPI_Init(argc, argv);
    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    return {world_size, world_rank};
}

std::tuple<std::vector<int>, std::vector<int>>
calculate_offset_and_batch_size(int world_size, int world_rank, int size);
std::vector<int> broadcast_data(int world_size, int world_rank, const std::vector<int> &full_vec);
int get_full_vec_size(const std::vector<int> &full_vec, int world_rank);

int main(int argc, char* argv[]) {
    try {
        auto [world_size, world_rank] = mpi_init(&argc, &argv);

        CommandLineArguments arguments{argc, argv};
        auto named_options = arguments.get_options();

        auto input_filename = arguments.at(1);
        auto output_filename = arguments.at(2);

        std::vector<int> full_vec{};
        if (world_rank == 0) {
            full_vec = read_vec<int>(input_filename);
        }

        auto start = std::chrono::high_resolution_clock::now();

        auto batch = broadcast_data(world_size, world_rank, full_vec);
        auto full_vec_size = get_full_vec_size(full_vec, world_rank);

        if (world_size == 1 || full_vec_size < world_size) {
            std::sort(full_vec.begin(), full_vec.end(), std::less<>{});
        } else {
            std::string algo = named_options.get_option("algorithm");
            if (algo == "merging") {
                mpi_parallel_sort_merging(full_vec, batch, world_size, world_rank);
            } else if (algo == "HYPERcube") {
                mpi_parallel_sort(full_vec, batch, world_size, world_rank);
            } else {
                throw std::runtime_error("Unknown algorithm");
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        if (world_rank == 0) {
            write_vec(output_filename, full_vec);
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
        }

        MPI_Finalize();
    } catch (const std::exception& ex) {
        std::cerr << "Exception was occurred: " << ex.what() << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        MPI_Finalize();
    } catch (...) {
        std::cerr << "Weird stuff was thrown" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        MPI_Finalize();
    }

    return 0;
}

std::tuple<std::vector<int>, std::vector<int>>
calculate_offset_and_batch_size(int world_size, int world_rank, int size) {
    std::vector<int> offsets(world_size);
    std::vector<int> batch_sizes(world_size);

    if (world_rank == 0) {
        int one_job_size = size / world_size;
        int odd_job = size % world_size;
        for (int i = 0; i < world_size; ++i) {
            batch_sizes[i] = i < odd_job ? one_job_size + 1 : one_job_size;
        }
        for (int i = 1; i < world_size; ++i) {
            offsets[i] = offsets[i - 1] + batch_sizes[i - 1];
        }
    }

    MPI_Bcast(offsets.data(), offsets.size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(batch_sizes.data(), batch_sizes.size(), MPI_INT, 0, MPI_COMM_WORLD);

    return {std::move(offsets), std::move(batch_sizes)};
}

std::vector<int> broadcast_data(int world_size, int world_rank, const std::vector<int> &full_vec) {
    auto [offsets, batch_sizes] = calculate_offset_and_batch_size(world_size, world_rank, full_vec.size());
    int my_batch_size = batch_sizes.at(world_rank);
    std::vector<int> batch(my_batch_size);
    MPI_Scatterv(full_vec.data(), batch_sizes.data(), offsets.data(), MPI_INT,
                 batch.data(), my_batch_size, MPI_INT, 0, MPI_COMM_WORLD);
    return std::move(batch);
}

int get_full_vec_size(const std::vector<int> &full_vec, int world_rank) {
    int full_vec_size = 0;
    if (world_rank == 0) {
        full_vec_size = full_vec.size();
    }
    MPI_Bcast(&full_vec_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    return full_vec_size;
}
