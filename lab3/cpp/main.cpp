#include <iostream>
#include <mpi.h>
#include <chrono>
#include <thread>
#include "command_line.h"

std::tuple<int, int> mpi_init(int* argc, char*** argv) {
    MPI_Init(argc, argv);
    int world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    return {world_size, world_rank};
}

int main(int argc, char* argv[]) {
    try {
        auto [world_size, world_rank] = mpi_init(&argc, &argv);

        CommandLineArguments arguments{argc, argv};
        auto named_options = arguments.get_options();

        auto input_filename = arguments.at(1);
        auto output_filename = arguments.at(2);
        auto start = std::chrono::high_resolution_clock::now();

        auto end = std::chrono::high_resolution_clock::now();
        if (world_rank == 0) {
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