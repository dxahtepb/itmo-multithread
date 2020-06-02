import copy
import json
import os
import subprocess
from datetime import datetime

from lab1.generate import generate_matrix, write_matrix

if __name__ == '__main__':
    os.chdir('cmake-build-debug')

    left_height = 200
    left_width = 100000
    right_height = left_width
    right_width = 200

    matrix_a_path = f'../matrices/{left_height}x{left_width}.txt'
    matrix_b_path = f'../matrices/{right_height}x{right_width}_2.txt'
    matrix_result_path = f'../matrices/{left_height}x{right_width}result.txt'

    # noinspection PyUnreachableCode
    if True:
        matrix_a = generate_matrix(left_height, left_width, (0, 100))
        write_matrix(matrix_a_path, matrix_a)

        matrix_b = generate_matrix(left_width, left_height, (0, 100))
        write_matrix(matrix_b_path, matrix_b)

    options = {
        'thread_num': -1,
        # 'chunk_size': -1,
        'runs': 2,
    }
    run_mode = 'parallel'

    run_results = {
        'run_mode': run_mode,
        'executor': 'orc',
        'left_size': [left_height, left_width],
        'right_size': [right_height, right_width],
        'runs': []
    }

    for i in [6, 8, 10, 12]:
        options['thread_num'] = i

        options_list = []
        for item in options.items():
            options_list.append(str(item[0]))
            options_list.append(str(item[1]))

        try:
            stdout = subprocess.check_output(
                ['./lab1', matrix_a_path, matrix_b_path, run_mode,
                 '--options', *options_list]
            )
            run_time = int(str(stdout, encoding='utf-8').strip())
            # noinspection PyTypeChecker
            run_results['runs'].append({
                    'options': copy.deepcopy(options),
                    'run_time': run_time,
            })
            print(f'{left_height}x{right_width} {i}: {run_time}')
        except subprocess.CalledProcessError as err:
            print(f'process finished with error: {err}')

    try:
        try:
            os.mkdir('../runs')
        except FileExistsError:
            pass
        with open(f'../runs/run_{datetime.now().strftime("%m_%d_%Y_%H_%M_%S")}', 'w') as file:
            json.dump(run_results, file, indent=2)
    except IOError as err:
        print(f'Can not write results: {err}')
