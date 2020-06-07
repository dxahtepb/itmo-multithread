### How to build and run this app

1. Make sure all requirements are satisfied
2. cd to lab3
3. Run `cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ./`
4. Compile sources with `cmake --build ./ --target lab3 -- -j <num_of_jobs>`
5. To run Jacobi solver execute it with `mpiexec`: `mpiexec -np <num_of_processors> ./lab3 <options>`

Command-line options recognized by this solver:
* Required positional options
  - 1: vector input path
  - 2: sorted vector output path
* Optional named arguments (provided after key `--optional` in `key value` format)
  - algorithm: which sorting algorithm will be used
    - `merging` for tree-merging batched parallel quicksort (any number of processes)
    - `HYPERcube` for hypercube repartition parallel quicksort (number of processes must be a power of two)
  
Example:

`mpiexec -np 4 ./lab3 ./vectors/example.txt ./vectors/result.txt --options algorithm HYPERcube`