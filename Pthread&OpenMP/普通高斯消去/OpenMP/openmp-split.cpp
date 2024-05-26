#include <bits/stdc++.h>
#include "gendata.h"
#include <omp.h>

using namespace std;

#define NUM_THREADS 4


float** A;
int n;


void LU() {
    // Create threads outside the loop to avoid repeated thread creation and destruction, which affects program performance
    float tmp;
    int i, j, k;
    bool parallel = true;
    #pragma omp parallel if(parallel), num_threads(NUM_THREADS), private(i, j, k, tmp)
    for (k=0; k<n; k++) {
        // Serial division
        #pragma omp single
        {
            tmp = A[k][k];
            // Since tmp is used, A[k][k] can also be included in the loop variable
            for (j=k; j<n; j++) {
                A[k][j] /= tmp;
            }
        }

        // Vertical task decomposition
        // #pragma omp for simd
        #pragma omp for
        for (j = k + 1; j < n; j++) {
            tmp = A[k][j];
            for (i = k + 1; i < n; i++) {
                A[i][j] -= A[i][k] * tmp;
            }
        }
    }
}

int main() {
    vector<int> size = {200, 500, 1000, 2000, 3000};
    for (int i=0; i<5; i++) {
        n = size[i];

        A = new float*[n];
        for (int i=0; i<n; i++) {
            A[i] = new float[n];
        }
        reset(A, n);

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