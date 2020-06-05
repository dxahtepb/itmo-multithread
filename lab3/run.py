import copy
import json
import os
import random
import subprocess
from datetime import datetime


def generate(size):
    lst = list(range(1, size + 1))
    random.shuffle(lst)
    return lst


def write_vector(path, vector):
    with open(path, 'w') as output:
        output.write(' '.join(map(str, vector)) + '\n')


def run_tests():
    executable = './cmake-build-debug/lab3'
    mpiexec = '/opt/openmpi-4.0.3/bin/mpiexec'

    executable_named_params = {}

    mpi_options = {
        '-np': 1
    }

    run_results = {
        'executable': 'mpi_broadcast',
        'runs': []
    }

    for size in (100_000, 1_000_000, 10_000_000, 100_000_000):
        matrix_path = f'./vectors/{size}_matrix.txt'
        write_vector(matrix_path, generate(size))

        for thread_num in (1, 2, 4, ):
            output = f'./vectors/{size}_{thread_num}_result.txt'

            mpi_options['-np'] = thread_num

            options_list = make_options_list(executable_named_params)
            mpi_options_list = make_options_list(mpi_options)

            try:
                proc = subprocess.Popen([mpiexec, *mpi_options_list, executable, matrix_path, output],
                                        stderr=subprocess.PIPE, stdout=subprocess.PIPE)
                out, err = proc.communicate()
                print(f'{size} {thread_num}: {str(out.strip(), encoding="utf-8")}')
                print(f'stderr: {str(err, encoding="utf-8")}')

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
