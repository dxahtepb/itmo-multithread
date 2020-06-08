import copy
import json
import os
import random
import subprocess
from datetime import datetime


def generate_full_graph(size):
    lst = [[0 for _ in range(size)] for _ in range(size)]
    for i in range(size):
        for j in range(i + 1, size):
            lst[j][i] = lst[i][j] = random.randint(1, 1000)
    return lst


def write_matrix(path, matrix):
    with open(path, 'w') as output:
        output.write(f'{len(matrix)}\n')
        for row in matrix:
            output.write(' '.join(map(str, row)) + '\n')


def run_tests():
    executable = './cmake-build-debug/lab4'

    run_results = {
        'executable': 'omp',
        'runs': []
    }

    for size in (10, 100, 300, 500, 800, 1_000, 2_000, 4_000, 5_000, 10_000, 20_000):
        matrix_path = f'./matrices/{size}_matrix.txt'
        if not os.path.exists(matrix_path):
            write_matrix(matrix_path, generate_full_graph(size))

        for thread_num in (1, 2, 4, 6, 8, 10, 12):
            output = f'./matrices/{size}_{thread_num}_result.txt'
            start_element = 0

            executable_named_params = {
                'run_mode': 'parallel',
                'run_times': 3,
                'num_threads': thread_num,
                'schedule': 'static',
                'chunk_size': -1,
            }

            options_list = make_options_list(executable_named_params)

            try:
                proc = subprocess.Popen([executable, matrix_path, str(start_element),
                                         output, '--options', *options_list],
                                        stderr=subprocess.PIPE, stdout=subprocess.PIPE)
                out, err = proc.communicate()
                print(f'{size} {thread_num}: {str(out.strip(), encoding="utf-8")}')
                print(f'stderr: {str(err, encoding="utf-8")}')
                run_time = int(out)

                run_results['runs'].append({
                    'thread_num': thread_num,
                    'size': size,
                    'run_time': run_time,
                    'named_params': copy.deepcopy(executable_named_params)
                })

            except subprocess.CalledProcessError as err:
                print(f'process finished with error: {err}')

    return run_results


def make_options_list(options):
    options_list = []
    for item in options.items():
        options_list.append(str(item[0]))
        options_list.append(str(item[1]))
    return options_list


def write_run_results(run_results, sub_folder='misc'):
    try:
        os.makedirs(f'./runs/{sub_folder}', exist_ok=True)
        with open(f'./runs/{sub_folder}/run_{datetime.now().strftime("%m_%d_%Y_%H_%M_%S")}', 'w') as file:
            json.dump(run_results, file, indent=2)
    except IOError as err:
        print(f'Can not write results: {err}')


def main():
    # run_tests()
    write_run_results(run_tests(), 'test_runs')


if __name__ == '__main__':
    main()
