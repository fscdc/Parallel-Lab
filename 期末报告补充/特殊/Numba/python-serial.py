import numpy as np
import time

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
example = 3

# Get corresponding parameter values based on example number
params = example_params[example]
num_columns = params["num_columns"]
num_elimination_rows = params["num_elimination_rows"]
num_eliminated_rows = params["num_eliminated_rows"]

# Get array length based on parameter values
length = int(np.ceil(num_columns / 5.0))

# Use numpy arrays for storage, R: elimination elements, E: eliminated rows
R = np.zeros((10000, length), dtype=np.uint32)
E = np.zeros((num_eliminated_rows, length), dtype=np.uint32)

# Record whether lifted
lifted = np.zeros(num_eliminated_rows, dtype=bool)

# Print the current bitmap to the screen
def print_bitmap():
    for i in range(num_eliminated_rows):
        print(f"{i}: ", end="")
        for j in range(num_columns - 1, -1, -1):
            if ((E[i][j // 5] >> (j - 5 * (j // 5))) & 1) == 1:
                print(j, end=" ")
        print()

# Read input
def input_data():
    # Read elimination elements
    file_R = open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例{} 矩阵列数{}，非零消元子{}，被消元行{}/消元子.txt".format(example, num_columns, num_elimination_rows, num_eliminated_rows), "r")
    for line in file_R:
        bits = list(map(int, line.split()))
        first_in = True
        index = 0
        for bit in bits:
            if first_in:
                first_in = False
                index = bit
            R[index][bit // 5] |= 1 << (bit - (bit // 5) * 5)
    file_R.close()

    # Read eliminated rows
    file_E = open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例{} 矩阵列数{}，非零消元子{}，被消元行{}/被消元行.txt".format(example, num_columns, num_elimination_rows, num_eliminated_rows), "r")
    index = 0
    for line in file_E:
        bits = list(map(int, line.split()))
        for bit in bits:
            E[index][bit // 5] |= 1 << (bit - (bit // 5) * 5)
        index += 1
    file_E.close()

# Serial Gaussian elimination using numpy array implementation
def solve():
    start = time.time()

    # Traverse eliminated rows: eliminate element by element, clear at once
    for i in range(num_eliminated_rows):
        is_eliminated = False
        # Traverse within the row to find the leading element to eliminate
        for j in range(length - 1, -1, -1):
            for k in range(4, -1, -1):
                if (E[i][j] >> k) == 1:
                    # Get the leading element
                    leader = 5 * j + k
                    if R[leader][j] != 0:
                        # Eliminate if there is an elimination element, need to XOR the entire row
                        for m in range(j, -1, -1):
                            E[i][m] ^= R[leader][m]
                    else:
                        # Otherwise, lift and after lifting, this entire row can be ignored
                        R[leader][:j + 1] = E[i][:j + 1]
                        is_eliminated = True
                        break
            if is_eliminated:
                break

    end = time.time()
    print(f"{end - start}s")

if __name__ == '__main__':
    input_data()
    solve()
