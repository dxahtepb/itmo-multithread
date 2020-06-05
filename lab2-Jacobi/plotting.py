import json
import os

import matplotlib.pyplot as plt


def read_json(filename):
    with open(filename) as file:
        return json.load(file)


def plot():
    parent_dir = 'runs/test_runs/'
    filenames = os.listdir(parent_dir)

    run_times_for_sizes = {}
    for filename in filenames:
        run_results = read_json(parent_dir + filename)
        for run_result in run_results['runs']:
            thread_num = f'{run_result["thread_num"]} thread(s)'
            if thread_num not in run_times_for_sizes:
                run_times_for_sizes[thread_num] = []
            run_times_for_sizes[thread_num].append((
                run_result['size'],
                run_result['run_time']
            ))

    fig = plt.figure()
    ax = fig.add_subplot(111)

    for line_label in run_times_for_sizes:
        run_times_for_sizes[line_label] = sorted(run_times_for_sizes[line_label])

    for line_label, runtimes_for_size in sorted(run_times_for_sizes.items()):
        sizes = [f'{x[0]}' for x in runtimes_for_size]
        speedups = [run_times_for_sizes['1 thread(s)'][i][1] / x[1] for i, x in enumerate(runtimes_for_size)]
        ax.plot(sizes, speedups, label=line_label)

    plt.ylabel('Speedup, times')
    plt.xlabel('Sizes')
    lgd = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), fancybox=True, ncol=2)

    os.makedirs('./images/', exist_ok=True)
    plt.savefig('./images/jacobi_2.png', bbox_extra_artists=(lgd, ), bbox_inches='tight')


def main():
    plot()


if __name__ == '__main__':
    main()
