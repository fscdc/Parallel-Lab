#include <iostream>
#include <chrono>
#include <immintrin.h>  // AVX、AVX2
#include "gendata.h"

using namespace std;

// AVX-PARALLEL
void LU(float** A, int N) {
    for (int k=0; k<N; k++) {
        // Couldn't find a convenient instruction similar to _mm_load1_ps in the AVX instruction set, 
        // so I used the method of creating a new array to copy 8 copies of A[k][k] into vt.
        float dup[8] = {A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k]};

        __m256 vt = _mm256_loadu_ps(dup);
        int j;
        for (j=k+1; j+8<=N; j+=8) {
            __m256 va = _mm256_loadu_ps(&A[k][j]);  // load，unaligned load

            va = _mm256_div_ps(va, vt);

            _mm256_storeu_ps(&A[k][j], va); // store，unaligned store
        }

        for (; j<N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i=k+1; i<N; i++) {
            float dupik[8] = {A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k]};
            __m256 vaik = _mm256_loadu_ps(dupik);
            int j;
            for (j=k+1; j+8<=N; j+=8) {
                // origin: A[i][j] = A[i][j] - A[k][j]*A[i][k];
                __m256 vakj = _mm256_loadu_ps(&A[k][j]);
                __m256 vaij = _mm256_loadu_ps(&A[i][j]);
                __m256 vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&A[i][j], vaij);
            }

            for (; j<N; j++) {
                A[i][j] = A[i][j] - A[k][j]*A[i][k];
            }
            A[i][k] = 0;
        }
    }
}

int main() {
    int N;
    vector<int> size = {200, 500, 1000, 2000, 3000};
    for (int i=0; i<5; i++) {
        N = size[i];

        float** A = new float*[N];
        for (int i=0; i<N; i++) {
            A[i] = new float[N];
        }

        reset(A, N);

        auto start = chrono::high_resolution_clock::now();
        LU(A, N);
        auto end = chrono::high_resolution_clock::now();
        auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
        cout << "Size = " << N << ": " << diff.count() << "ms" << endl;
    }
    return 0;
}