from random import randint
from typing import Tuple, List


Matrix = List[List[int]]


def generate_matrix(height: int, width: int, range_: Tuple[int, int]) -> Matrix:
    return [[randint(*range_) for _ in range(width)] for _ in range(height)]


def generate_identity(height: int) -> List[List[int]]:
    return [[1 if column == row else 0 for column in range(height)] for row in range(height)]


def write_matrix(filename: str, matrix: Matrix):
    with open(filename, 'w') as output:
        output.write(f'{len(matrix)} {len(matrix[0])}\n')
        for row in matrix:
            output.write(' '.join(map(str, row)) + '\n')


if __name__ == '__main__':
    matrix_a = generate_matrix(2048, 2048, (0, 100))
    matrix_b = generate_identity(2048)
    write_matrix('matrices/2048x2048.txt', matrix_a)
    write_matrix('matrices/2048x2048identity.txt', matrix_b)
