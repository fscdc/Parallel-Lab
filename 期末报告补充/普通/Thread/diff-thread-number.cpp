#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "gendata.h"

using namespace std;

// Eliminate the rows below the k-th row
void LU(float** A, int n, int k, int start, int end) {
    for (int i = start; i < end; i++) {
        for (int j = k + 1; j < n; j++) {
            A[i][j] -= A[k][j] * A[i][k];
        }
        A[i][k] = 0;
    }
}

int main() {
    int size[] = { 200, 500, 1000, 2000, 3000 };
    for (int i = 0; i < 5; i++) {
        int n = size[i];

        // Initialize the 2D array and generate random numbers
        float** A = new float* [n];
        for (int i = 0; i < n; i++) {
            A[i] = new float[n];
        }

        // Reset the array
        reset(A, n);

        // Use the chrono library from C++11 to measure time
        auto start = chrono::high_resolution_clock::now();

        // Parallelize the process
        int num_threads = thread::hardware_concurrency();
        cout << "Number of threads: " << num_threads << endl;

        for (int k = 0; k < n; k++) {
            // Perform division operation serially
            for (int j = k + 1; j < n; j++) {
                A[k][j] /= A[k][k];
            }
            A[k][k] = 1.0;

            vector<thread> threads;
            
            // Divide into chunks
            int chunk_size = (n - k) / num_threads;
            for (int t = 0; t < num_threads; t++) {
                int start = k + 1 + t * chunk_size;
                int end = (t == num_threads - 1) ? n : (start + chunk_size);
                threads.emplace_back([&]() { LU(A, n, k, start, end); });
            }

            // Wait for all threads to finish
            for (auto& t : threads) {
                t.join();
            }
        }

        auto end = chrono::high_resolution_clock::now();
        auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
        cout << "Size = " << n << ": " << diff.count() << "ms" << endl;

        print(A, n);
        break;

        // Free the memory of the 2D array A
        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        delete[] A;
    }
    return 0;
}
