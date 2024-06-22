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
example = 5

# Get corresponding parameter values based on example number
params = example_params[example]
num_columns = params["num_columns"]
num_elimination_rows = params["num_elimination_rows"]
num_eliminated_rows = params["num_eliminated_rows"]

# Get array length based on parameter values
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

@njit(parallel=True)
def solve(E, R, length, num_columns, num_eliminated_rows):
    # Process the eliminated rows without lifting, process 8 elimination subscripts per round, range: from i-7 to i
    for i in range(length - 1, -2, -8):
        sign = True
        while sign:
            # Traverse each row of the eliminated rows
            for j in range(num_eliminated_rows):
                index = R[j][num_columns]
                while i - 7 <= R[j][num_columns] <= i:
                    if E[index][num_columns] == 1:
                        # Eliminate
                        for k in range(num_columns):
                            R[j][k] ^= E[index][k]

                        num = 0
                        S_num = 0
                        for num in range(num_columns):
                            if R[j][num] != 0:
                                temp = R[j][num]
                                while temp != 0:
                                    temp = temp >> 1
                                    S_num += 1
                                S_num += num * 32
                                break
                        R[j][num_columns] = S_num - 1
                    else:
                        break

                # Then re-judge whether to end, if not, lift the corresponding elimination subscripts
                sign = False
                for i in range(num_eliminated_rows):
                    temp = R[i][num_columns]
                    if temp == -1:
                        continue

                    if E[temp][num_columns] == 0:
                        for k in range(num_columns):
                            E[temp][k] = R[i][k]
                        R[i][num_columns] = -1
                        sign = True

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
solve(E, R, length, num_columns, num_eliminated_rows)
end = time.time()
diff = (end - start) * 1000
print(diff, "ms")

# Verify the correctness of the result
print_bitmap()
