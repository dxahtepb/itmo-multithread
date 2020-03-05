import os
import subprocess

from lab1.generate import generate_matrix, write_matrix, generate_identity


if __name__ == '__main__':
    os.chdir('./cmake-build-debug')

    size = 1024
    matrix_a_path = f'../matrices/{size}x{size}.txt'
    matrix_b_path = f'../matrices/{size}x{size}identity.txt'
    matrix_result_path = f'../matrices/{size}x{size}result.txt'

    matrix_a = generate_matrix(size, size, (0, 100))
    write_matrix(matrix_a_path, matrix_a)

    matrix_b = generate_identity(size)
    write_matrix(matrix_b_path, matrix_b)

    stdout = subprocess.check_output(['./lab1', matrix_a_path, matrix_b_path, matrix_result_path, 'parallel'])
    print(stdout)
