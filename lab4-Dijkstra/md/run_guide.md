### How to build and run this app

1. Make sure all requirements are satisfied
2. cd to lab4-Dijkstra
3. Run `cmake -DCMAKE_BUILD_TYPE=Debug ./`
4. Compile sources with `cmake --build ./ --target lab4 -- -j <num_of_jobs>`
5. To run multiplier execute it with `./lab4 <options>`

Command-line options recognized by this solver:
* Required positional options
  - 1: input file with weighted incidence matrix path
  - 2: start vertex (compute distances from it)
  - 3: output file path
* Optional named arguments (provided after key `--optional` in `key value` format)
  - run mode, one of
       - `sequential` for sequential algorithm [default]
       - `parallel` for parallel openmp algorithm
  - thread_num: number of used threads (default openmp decides by itself)
  - run_times: number of runs to average running time (default 1)
  - schedule: schedule for omp parallel for (default `static`)
  - chunk_size: chunk size for omp parallel for (default -1)

Example run:

```
./lab4 ./matrices/example.txt 0 ./matrices/result.txt.example --options \
    thread_num 12 \
    run_mode parallel \
    run_times 3
```