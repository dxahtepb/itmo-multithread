## Lab 1 Parallel Matrix Multiplication with OpenMP

Requirements:
* gcc (9+ for stable std::filesystem)
* cmake (3.14+)
* openmp (part of gcc-9)
* python3 (3.6+ for f-strings)
  * matplotlib
  
System info:
* OS: Ubuntu Linux 18.04
* CPU info: [lscpu output](md/cpu_characteristics.md)

Benchmark results could be found on [GDrive folder](https://drive.google.com/drive/folders/19qHRaOD4-fhEXwkaPt3r8iA10EfreSiD?usp=sharing).

Build and run guide [link](./md/run.md).

For multiplication A(n,k) * B(k, m) standard 3-loops O(n * k * m) algorithm are used. Iterations with total length of n * m are distributed between workers. Each worker computes its respective part of the resulting matrix C. Since A and B are read-only and every element Cij of C is calculated independently, there is no contention and dataraces between worker threads.

### Speedup with different schedulers comparison 
OpenMP provides different scheduling strategies which determine how iterations of the parallelized loop will be assigned to worker threads.
`Static` scheduler uniformly distributes iterations between threads, while `Dynamic` and `Guided` schedulers are useful for workloads when each thread job is highly unbalanced and their computation takes a very inconsistent time. 
Matrix multiplication run time is T(n) = O(batch_size*k) for each job. Thus the best resources utilization could be achieved with equal distribution of batches between threads.
The same as theory states, no significant difference was noticed in benchmarks for different scheduling types.

![Image](./images/scheduling_types.png)

Speedup is relative to the sequential run for the same matrix of the same size.
All 12 logical cores are used for this benchmark. For dynamic and guided schedulers batch_size was set to `iterations / num_threads`.
With different distributions (batch sizes) performance dramatically drops.

### Column-oriented right matrix optimization

One widely known optimization for matrix multiplication is to make it more CPU cache aware. 
The problem is that right (`B`) matrix is traversed in column-first order, resulting in cache miss on every iteration for big enough `k`.
Optimization resides in storing right matrix in column-row order, this gains a significant boost in performance for particular matrices.
This effect could be seen on the following graph, where for bigger `k` runtime of column-oriented version is much more low.
While for relatively small 200, which is somewhat less than L1 cache size (moreover presuming preloading), run time is equal.

![Image](./images/orc_vs_ocr.png)

### Speedup on number of threads dependence

From this graph, we may make a conclusion that logical cores are slightly slower than physical ones. 
And speedup is mostly monotonic (with some special points) and linear.

![Image](./images/parallel_cores_speedup.png)