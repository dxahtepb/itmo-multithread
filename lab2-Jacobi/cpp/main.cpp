#include <iostream>
#include "command_line.h"
#include "mpi.h"
#include "jacobi.h"
#include "util.h"
#include "LinearSystem.h"

std::tuple<int, int> mpi_init(int* argc, char*** argv) {
    MPI_Init(argc, argv);
    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    return {world_size, world_rank};
}

void mpi_finalize() {
    MPI_Finalize();
}

void read_data(int world_rank, const std::string& matrix_file_name,
               const std::string& initial_approximation_file_name, LinearSystem* d) {
    if (world_rank == 0) {
        d->size = read_matrix(matrix_file_name, d->matrix, d->b);
        if (!is_strictly_diagonal_dominant(d->matrix, d->size)) {
            throw std::runtime_error("Matrix must be strictly diagonal dominant!");
        }
        auto x_size = read_initial_approximation(initial_approximation_file_name, d->x_initial);
        if (d->size != x_size) {
            throw std::runtime_error("Initial approximation has different cardinality!");
        }
    }
}

void broadcast_data(int world_rank, LinearSystem* d) {
    MPI_Bcast(&d->size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank != 0) {
        d->matrix.resize(d->size * d->size);
        d->b.resize(d->size);
        d->x_initial.resize(d->size);
    }
    MPI_Bcast(d->matrix.data(), d->size * d->size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(d->b.data(), d->size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(d->x_initial.data(), d->size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

std::tuple<std::vector<int>, std::vector<int>>
calculate_offset_and_batch_size(int world_size, int world_rank, int size) {
    std::vector<int> offsets(world_size);
    std::vector<int> batch_sizes(world_size);

    int one_job_size = size / world_size;
    int odd_job = size % world_size;
    for (int i = 0; i < world_size; ++i) {
        batch_sizes[i] = i < odd_job ? one_job_size + 1 : one_job_size;
    }
    for (int i = 1; i < world_size; ++i) {
        offsets[i] = offsets[i - 1] + batch_sizes[i - 1];
    }

    return {offsets, batch_sizes};
}

int main(int argc, char* argv[]) {
    try {
        auto [world_size, world_rank] = mpi_init(&argc, &argv);

        CommandLineArguments arguments{argc, argv};
        auto named_options = arguments.get_options();

        LinearSystem linear_system{};
        auto matrix_file_name = arguments.at(1);
        auto initial_approximation_file_name = arguments.at(2);
        auto output_file_name = arguments.at(3);
        auto eps = std::stod(arguments.at(4));
        auto max_iterations = arguments.get_options().get_int_option_or_default("max_iterations", 1000);

        read_data(world_rank, matrix_file_name, initial_approximation_file_name, &linear_system);

        auto start = std::chrono::high_resolution_clock::now();

        broadcast_data(world_rank, &linear_system);

        auto [offset, batch_size] = calculate_offset_and_batch_size(world_size, world_rank, linear_system.size);
        auto [jacobi_status, result] =
                jacobi_method_mpi(linear_system, max_iterations, eps, world_rank, offset, batch_size);

        auto end = std::chrono::high_resolution_clock::now();

        if (world_rank == 0) {
            if (!jacobi_status) {
                std::cerr << "Failed to achieve desired precision" << std::endl;
            }
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
            write_vector(output_file_name, result);
        }

        mpi_finalize();
    } catch (const std::exception& ex) {
        std::cerr << "Exception was occurred: " << ex.what() << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        mpi_finalize();
    } catch (...) {
        std::cerr << "Weird stuff was thrown" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        mpi_finalize();
    }

    return 0;
}