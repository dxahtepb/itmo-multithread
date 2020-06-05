### How to build and run this app

1. Make sure all requirements are satisfied
2. cd to lab2-Jacobi
3. Run `cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ./lab2-Jacobi`
4. Compile sources with `cmake --build ./lab2-Jacobi/cmake-build-debug --target lab2 -- -j <num_of_jobs>`
5. To run Jacobi solver execute it with `mpiexec`: `mpiexec -np <num_of_processors> ./lab2-Jacobi/lab2 <options>`

Command-line options recognized by this solver:
* Required positional options
  - 1: input matrix coefficients path
  - 2: input initial approximation path
  - 3: output path for resulting solution vector
  - 4: desired precision
* Optional named arguments (provided after key `--optional` in `key value` format)
  - max_iterations: maximal number of iterations solver will make
  
Example:

`mpiexec -np 6 ./lab2-Jacobi/lab2 ./matrices/example.txt ./matrices/example_init.txt ./matrices/result.txt 0.00001 --options max_iterations 10000`