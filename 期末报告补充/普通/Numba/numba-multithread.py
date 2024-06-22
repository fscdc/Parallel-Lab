import numpy as np
import numba as nb
import time

@nb.jit(nopython=True,parallel=True,fastmath=True)
def LU(A):
    n = A.shape[0]
    for k in range(n):
        for j in nb.prange(k + 1, n):
            A[k, j] /= A[k, k]
        A[k, k] = 1.0
        for i in nb.prange(k + 1, n):
            for j in range(k + 1, n):
                A[i, j] -= A[k, j] * A[i, k]
            A[i, k] = 0

def print_matrix(A):
    n = A.shape[0]
    for i in range(n):
        for j in range(n):
            print(A[i, j], end="\t")
        print()

sizes = [200, 500, 1000, 2000, 3000]
for n in sizes:
    # Initialize a 2D array and generate random numbers
    A = np.random.rand(n, n).astype(np.float32)

    # Perform LU decomposition using Numba parallel computation
    start = time.time()
    LU(A)
    end = time.time()
    diff = end - start
    print(f"Size = {n}: {diff * 1000}ms")

    # # Print and verify the computation result
    # print("Result:")
    # print_matrix(A)
    # print()
    # break
