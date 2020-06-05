### How to build and run this app

1. Make sure all requirements are satisfied
2. cd to lab1
3. Run `cmake -DCMAKE_BUILD_TYPE=Debug ./`
4. Compile sources with `cmake --build ./ --target lab1 -- -j <num_of_jobs>`
5. To run multiplier execute it with `./lab1 <options>`

Command-line options recognized by this solver:
* Required positional options
  - 1: input left matrix A path
  - 2: input right matrix B path
  - 3: run mode, one of
     - `sequential`
     - `parallel` for static scheduling
     - `parallel_dynamic` for dynamic scheduling
     - `parallel_guided` for guided scheduling
* Optional named arguments (provided after key `--optional` in `key value` format)
  - is_orc: `true` to enable column-order optimization (true by default)
  - runs: number of runs made to get average running time (1 by default)
  - thread_num: number of used threads (default openmp decides by itself)
  - chunk_size: chunks size (default openmp decides by itself)
  
Example run:

`./lab1 ./matrices/exampleA ./matrices/exampleB parallel --options is_orc true thread_num 12 runs 5`