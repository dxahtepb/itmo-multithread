import copy
import itertools
import json
import os
import subprocess
from datetime import datetime

from lab1.generate import generate_matrix, write_matrix


def run_cores_speedup_tests():
    executable = './cmake-build-debug/lab1'

    left_height = 2000
    left_width = 2000
    right_height = left_width
    right_width = 4000

    matrix_a_path = f'./matrices/{left_height}x{left_width}.txt'
    matrix_b_path = f'./matrices/{right_height}x{right_width}_2.txt'

    matrix_a = generate_matrix(left_height, left_width, (0, 1000))
    write_matrix(matrix_a_path, matrix_a)

    matrix_b = generate_matrix(right_height, right_width, (0, 1000))
    write_matrix(matrix_b_path, matrix_b)

    options = {
        'thread_num': -1,
        'chunk_size': -1,
        'runs': 3,
    }

    run_results = {
        'run_mode': 'parallel',
        'executor': 'orc',
        'left_size': [left_height, left_width],
        'right_size': [right_height, right_width],
        'runs': []
    }

    for run_mode, thread_num in itertools.product(
            ('parallel', ),
            (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, )
    ):
        options['thread_num'] = thread_num

        options_list = []
        for item in options.items():
            options_list.append(str(item[0]))
            options_list.append(str(item[1]))

        try:
            stdout = subprocess.check_output(
                [executable, matrix_a_path, matrix_b_path, run_mode,
                 '--options', *options_list]
            )
            run_time = int(str(stdout, encoding='utf-8').strip())
            # noinspection PyTypeChecker
            run_results['runs'].append({
                'options': copy.deepcopy(options),
                'run_time': run_time,
            })
            print(f'{left_height}x{right_width} {thread_num}: {run_time}')
        except subprocess.CalledProcessError as err:
            print(f'process finished with error: {err}')

    return run_results


def run_orc_vs_ocr_tests():
    executable = './cmake-build-debug/lab1'

    options = {
        'thread_num': -1,
        'chunk_size': -1,
        'runs': 3,
    }

    run_results = {
        'run_mode': 'parallel',
        'executor': 'various',
        'runs': []
    }

    for sizes, run_mode, thread_num in itertools.product(
            ((200, 10000, 200), (200, 50000, 200), (200, 100000, 200),
             (100, 500000, 100), (4000, 200, 4000), ),
            ('parallel', ),
            (12, )
    ):
        left_height = sizes[0]
        left_width = sizes[1]
        right_height = left_width
        right_width = sizes[2]

        matrix_a_path = f'./matrices/{left_height}x{left_width}.txt'
        matrix_b_path = f'./matrices/{right_height}x{right_width}_2.txt'

        matrix_a = generate_matrix(left_height, left_width, (0, 1000))
        write_matrix(matrix_a_path, matrix_a)

        matrix_b = generate_matrix(right_height, right_width, (0, 1000))
        write_matrix(matrix_b_path, matrix_b)

        options['thread_num'] = thread_num

        for is_orc in ('true', 'false'):
            options_list = []
            for item in options.items():
                options_list.append(str(item[0]))
                options_list.append(str(item[1]))

            try:
                stdout = subprocess.check_output(
                    [executable, matrix_a_path, matrix_b_path, run_mode,
                     '--options', *options_list]
                )
                run_time = int(str(stdout, encoding='utf-8').strip())
                # noinspection PyTypeChecker
                run_results['runs'].append({
                    'is_orc': is_orc,
                    'left_size': [left_height, left_width],
                    'right_size': [right_height, right_width],
                    'options': copy.deepcopy(options),
                    'run_time': run_time,
                })
                print(f'{left_height}x{right_width} {is_orc}: {run_time}')
            except subprocess.CalledProcessError as err:
                print(f'process finished with error: {err}')

        return run_results


def run_scheduling_type_tests():
    executable = './cmake-build-debug/lab1'

    options = {
        'thread_num': -1,
        'chunk_size': -1,
        'runs': 3,
    }

    run_results = {
        'run_mode': 'parallel',
        'executor': 'various',
        'runs': []
    }

    for sizes, thread_num in itertools.product(
            ((200, 200, 200), (500, 500, 500), (1000, 1000, 1000), (1500, 1500, 1500),
             (2000, 2000, 2000), (2500, 2500, 2500), (3000, 3000, 3000)),
            (12, )
    ):
        left_height = sizes[0]
        left_width = sizes[1]
        right_height = left_width
        right_width = sizes[2]

        matrix_a_path = f'./matrices/{left_height}x{left_width}.txt'
        matrix_b_path = f'./matrices/{right_height}x{right_width}_2.txt'

        matrix_a = generate_matrix(left_height, left_width, (0, 1000))
        write_matrix(matrix_a_path, matrix_a)

        matrix_b = generate_matrix(right_height, right_width, (0, 1000))
        write_matrix(matrix_b_path, matrix_b)

        options['thread_num'] = thread_num

        for run_mode in ('sequential', 'parallel', 'parallel_dynamic', 'parallel_guided', ):
            if run_mode in ('parallel_dynamic', 'parallel_guided'):
                options['chunk_size'] = ((left_height * right_width) // thread_num) + 1

            options_list = []
            for item in options.items():
                options_list.append(str(item[0]))
                options_list.append(str(item[1]))

            try:
                stdout = subprocess.check_output(
                    [executable, matrix_a_path, matrix_b_path, run_mode,
                     '--options', *options_list]
                )
                run_time = int(str(stdout, encoding='utf-8').strip())
                # noinspection PyTypeChecker
                run_results['runs'].append({
                    'run_mode': run_mode,
                    'left_size': [left_height, left_width],
                    'right_size': [right_height, right_width],
                    'run_time': run_time,
                })
                print(f'{left_height}x{right_width} {run_mode} {thread_num}: {run_time}')
            except subprocess.CalledProcessError as err:
                print(f'process finished with error: {err}')

    return run_results


def write_run_results(run_results, sub_folder='misc'):
    try:
        os.makedirs(f'./runs/{sub_folder}', exist_ok=True)
        with open(f'./runs/{sub_folder}/run_{datetime.now().strftime("%m_%d_%Y_%H_%M_%S")}', 'w') as file:
            json.dump(run_results, file, indent=2)
    except IOError as err:
        print(f'Can not write results: {err}')


def main():
    write_run_results(run_cores_speedup_tests(), 'parallel_cores_speedup')
    write_run_results(run_orc_vs_ocr_tests(), 'orc_vs_ocr')
    write_run_results(run_scheduling_type_tests(), 'scheduling_types')


if __name__ == '__main__':
    main()
