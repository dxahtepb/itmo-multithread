import copy
import json
import os
import random
import subprocess
from datetime import datetime


def generate(size):
    matrix = [[random.random() * 100 - 50 for _ in range(size)] for _ in range(size)]
    for i, row in enumerate(matrix):
        row[i] = sum(map(abs, row)) + random.random() * 100 + 20
        row.append(random.random() * 200 - 100)
    x_init = [random.random() for _ in range(size)]
    return matrix, x_init


def write_matrix(path, matrix):
    with open(path, 'w') as output:
        output.write(f'{len(matrix)} {len(matrix[0])}\n')
        for row in matrix:
            output.write(' '.join(map(str, row)) + '\n')


def write_vector(path, vector):
    with open(path, 'w') as output:
        output.write(f'{len(vector)}\n')
        output.write('\n'.join(map(str, vector)) + '\n')


def run_tests():
    executable = './cmake-build-debug/lab2'
    mpiexec = '/opt/openmpi-4.0.3/bin/mpiexec'

    eps = 1e-5

    executable_named_params = {
        'max_iterations': 100000,
    }

    mpi_options = {
        '-np': 1
    }

    run_results = {
        'executable': 'mpi_broadcast',
        'eps': eps,
        'runs': []
    }

    for size in (100, 200, 500, 1000, 2000, 3000, 5000):
        matrix_path = f'./matrices/{size}_matrix.txt'
        initial_path = f'./matrices/{size}_initial.txt'

        matrix, initial = generate(size)
        write_matrix(matrix_path, matrix)
        write_vector(initial_path, initial)

        for thread_num in (1, 2, 4, 6):
            output = f'./matrices/{size}_{thread_num}_result.txt'

            mpi_options['-np'] = thread_num

            options_list = make_options_list(executable_named_params)
            mpi_options_list = make_options_list(mpi_options)

            try:
                proc = subprocess.Popen([mpiexec, *mpi_options_list, executable, matrix_path,
                                        initial_path, output, str(eps), '--options', *options_list],
                                        stderr=subprocess.PIPE, stdout=subprocess.PIPE)
                out, err = proc.communicate()
                print(f'{size} {thread_num}: {str(out.strip(), encoding="utf-8")}')
                print(f'stderr: {str(err, encoding="utf-8")}')

                if out:
                    run_time = int(out)
                    # noinspection PyTypeChecker
                    run_results['runs'].append({
                        'thread_num': thread_num,
                        'mpi_options': copy.deepcopy(mpi_options),
                        'size': size,
                        'run_time': run_time,
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
    write_run_results(run_tests(), 'test_runs')


if __name__ == '__main__':
    main()
