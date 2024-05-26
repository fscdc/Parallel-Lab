#include <iostream>
#include <chrono>
#include <vector>
#include <omp.h>

using namespace std;

#define NUM_THREADS 4

// Cache line size, determined by actual hardware
#define CACHE_LINE_SIZE 64

// Padding structure
struct alignas(CACHE_LINE_SIZE) MyStruct {
    float data;
    char padding[CACHE_LINE_SIZE - sizeof(float)];  // Padding variable
};

// Data used for filling to solve false sharing problem
MyStruct** A;
int n;

// OpenMP Gaussian elimination, LU decomposition
void LU() {
    float tmp;
    int i, j, k;
    bool parallel = true;

    #pragma omp parallel if(parallel) num_threads(NUM_THREADS) private(i, j, k, tmp)
    for (k = 0; k < n; k++) {
        #pragma omp single
        {
            tmp = A[k][k].data;
            for (j = k; j < n; j++) {
                A[k][j].data /= tmp;
            }
        }

        // #pragma omp for simd
        #pragma omp for
        for (i = k + 1; i < n; i++) {
            tmp = A[i][k].data;
            for (j = k; j < n; j++) {
                A[i][j].data -= A[k][j].data * tmp;
            }
        }
    }
}

int main() {
    vector<int> size = {200, 500, 1000, 2000, 3000};
    for (int i = 0; i < 5; i++) {
        n = size[i];

        A = new MyStruct*[n];
        for (int i = 0; i < n; i++) {
            A[i] = new MyStruct[n];
        }

        auto start = chrono::high_resolution_clock::now();
        LU();
        auto end = chrono::high_resolution_clock::now();
        auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
        cout << "Size = " << n << ": " << diff.count() << "ms" << endl;

        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        delete[] A;
    }
    return 0;
}