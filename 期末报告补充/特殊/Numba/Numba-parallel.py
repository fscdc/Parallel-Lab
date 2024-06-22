import numpy as np
from numba import njit, prange

# Define example parameter dictionary
example_params = {
    3: {
        "num_columns": 562,
        "num_elimination_rows": 170,
        "num_eliminated_rows": 53
    },
    4: {
        "num_columns": 1011,
        "num_elimination_rows": 539,
        "num_eliminated_rows": 263
    },
    5: {
        "num_columns": 2362,
        "num_elimination_rows": 1226,
        "num_eliminated_rows": 453
    },
    6: {
        "num_columns": 3799,
        "num_elimination_rows": 2759,
        "num_eliminated_rows": 1953
    },
    7: {
        "num_columns": 8399,
        "num_elimination_rows": 6375,
        "num_eliminated_rows": 4535
    }
}

# Input example number
example = 6

# Get corresponding parameter values based on example number
params = example_params[example]
num_columns = params["num_columns"]
num_elimination_rows = params["num_elimination_rows"]
num_eliminated_rows = params["num_eliminated_rows"]

# Calculate array length based on parameter values
length = int(np.ceil(num_columns / 5.0))

# Use numpy arrays to store R and E
R = np.zeros((10000, length), dtype=np.uint8)
E = np.zeros((num_eliminated_rows, length), dtype=np.uint8)

# Record whether it is lifted
lifted = np.zeros(num_eliminated_rows, dtype=bool)

# Print the current bitmap to the screen
def print_bitmap():
    for i in range(num_eliminated_rows):
        for j in range(num_columns - 1, -1, -1):
            if ((E[i][j // 5] >> (j - 5 * (j // 5))) & 1) == 1:
                print(j, end=' ')
        print()

# Numba parallelization for special Gaussian elimination
@njit(parallel=True)
def solve(E, R, length, num_columns, num_eliminated_rows, lifted):
    for n in range(length - 1, -1, -1):
        records = []
        for i in range(num_eliminated_rows):
            if lifted[i]:
                continue
            for j in range(5 * n + 4, 5 * n - 1, -1):
                if (E[i][n] >> (j - 5 * n)) == 1:
                    # If there is a corresponding elimination element, eliminate it
                    if R[j][n] != 0:
                        E[i][n] ^= R[j][n]
                        records.append((i, j))
                    # Otherwise, perform lifting operation and immediately lift
                    else:
                        for pair in records:
                            row = pair[0]
                            leader = pair[1]
                            if row == i:
                                for k in range(n - 1, -1, -1):
                                    E[i][k] ^= R[leader][k]
                        R[j][:n + 1] = E[i][:n + 1]
                        lifted[i] = True
                        break

        # Parallelize based on records
        for m in prange(n - 1):
            for pair in records:
                row = pair[0]
                leader = pair[1]
                if lifted[row]:
                    continue
                E[row][m] ^= R[leader][m]

# Read R elements
with open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例{} 矩阵列数{}，非零消元子{}，被消元行{}/消元子.txt".format(example, num_columns, num_elimination_rows, num_eliminated_rows), "r") as file_R:
    for line in file_R:
        bits = list(map(int, line.split()))
        index = bits[0]
        for bit in bits[1:]:
            R[index][bit // 5] |= 1 << (bit - (bit // 5) * 5)

# Read E elements
with open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例{} 矩阵列数{}，非零消元子{}，被消元行{}/被消元行.txt".format(example, num_columns, num_elimination_rows, num_eliminated_rows), "r") as file_E:
    index = 0
    for line in file_E:
        bits = list(map(int, line.split()))
        for bit in bits:
            E[index][bit // 5] |= 1 << (bit - (bit // 5) * 5)
        index += 1

# Time and run Gaussian elimination
import time
start = time.time()
solve(E, R, length, num_columns, num_eliminated_rows, lifted)
end = time.time()
diff = end - start
print(diff, "s")

# Verify the correctness of the result
# print_bitmap()
