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
