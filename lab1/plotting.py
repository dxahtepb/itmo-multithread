import json
import os

import matplotlib.pyplot as plt


def read_json(filename):
    with open(filename) as file:
        return json.load(file)


def plot_parallel_cores_speedup():
    parent_dir = 'runs/parallel_cores_speedup/'
    filenames = os.listdir(parent_dir)

    lines = {}
    for filename in filenames:
        run_results = read_json(parent_dir + filename)
        sizes = (run_results['left_size'], run_results['right_size'])
        runtimes = [x['run_time'] for x in run_results['runs']]
        speedups = [runtimes[0] / x for x in runtimes]
        lines[str(sizes)] = speedups

    fig = plt.figure()
    ax = fig.add_subplot(111)

    cores = list(range(1, 13))
    for line_label, speedups in sorted(lines.items()):
        ax.plot(cores, speedups, label=line_label)

    plt.ylabel('Speedup, times')
    plt.xlabel('Cores')
    lgd = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), fancybox=True, ncol=2)

    plt.savefig('./images/parallel_cores_speedup.png', bbox_extra_artists=(lgd, ), bbox_inches='tight')


def plot_parallel_orc_vs_ocr():
    parent_dir = 'runs/orc_vs_ocr/'
    filenames = os.listdir(parent_dir)

    lines = {}
    for filename in filenames:
        run_results = read_json(parent_dir + filename)
        runtimes_for_size = sorted([
            (
                (x['left_size'], x['right_size']),
                x['run_time']
            )
            for x in run_results['runs']], key=lambda x: x[1])
        lines[run_results['executor']] = runtimes_for_size

    fig = plt.figure()
    ax = fig.add_subplot(111)

    for line_label, runtimes_for_size in sorted(lines.items()):
        sizes = [f'{x[0][0]}\n{x[0][1]}' for x in runtimes_for_size]
        runtimes = [x[1] for x in runtimes_for_size]
        ax.plot(sizes, runtimes, label=line_label)

    plt.ylabel('Run time, ms')
    plt.xlabel('Sizes')
    lgd = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), fancybox=True, ncol=2)

    plt.savefig('./images/orc_vs_ocr.png', bbox_extra_artists=(lgd, ), bbox_inches='tight')


def plot_scheduling_types():
    parent_dir = 'runs/scheduling_types/'
    filenames = os.listdir(parent_dir)

    run_times_for_sizes = {}
    for filename in filenames:
        run_results = read_json(parent_dir + filename)
        for run_result in run_results['runs']:
            run_mode = run_result['run_mode']
            if run_mode not in run_times_for_sizes:
                run_times_for_sizes[run_mode] = []
            run_times_for_sizes[run_mode].append((
                (run_result['left_size'], run_result['right_size']),
                run_result['run_time']
            ))

    fig = plt.figure()
    ax = fig.add_subplot(111)

    for line_label in run_times_for_sizes:
        run_times_for_sizes[line_label] = sorted(run_times_for_sizes[line_label])

    for line_label, runtimes_for_size in sorted(run_times_for_sizes.items()):
        if line_label == 'sequential':
            continue
        sizes = [f'{x[0][0]}\n{x[0][1]}' for x in runtimes_for_size]
        speedups = [run_times_for_sizes['sequential'][i][1] / x[1] for i, x in enumerate(runtimes_for_size)]
        ax.plot(sizes, speedups, label=line_label)

    plt.ylabel('Speedup, times')
    plt.xlabel('Sizes')
    lgd = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), fancybox=True, ncol=2)

    plt.savefig('./images/scheduling_types.png', bbox_extra_artists=(lgd, ), bbox_inches='tight')


def main():
    plot_parallel_cores_speedup()
    plot_parallel_orc_vs_ocr()
    plot_scheduling_types()


if __name__ == '__main__':
    main()
