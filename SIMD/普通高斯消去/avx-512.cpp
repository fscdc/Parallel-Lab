#include <iostream>
#include <chrono>
#include <immintrin.h>  // AVX、AVX2
#include "gendata.h"

using namespace std;

// AVX512-PARALLEL
void LU(float** A, int N) {
    for (int k=0; k<N; k++) {
        float dup[16] = {A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k]};

        __m512 vt = _mm512_loadu_ps(dup);
        int j;
        for (j=k+1; j+16<=N; j+=16) {
            __m512 va = _mm512_loadu_ps(&A[k][j]); 

            va = _mm512_div_ps(va, vt);

            _mm512_storeu_ps(&A[k][j], va);
        }

        for (; j<N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i=k+1; i<N; i++) {
            float dupik[16] = {A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k]};
            __m512 vaik = _mm512_loadu_ps(dupik);
            int j;
            for (j=k+1; j+16<=N; j+=16) {
                // origin code: A[i][j] = A[i][j] - A[k][j]*A[i][k];
                __m512 vakj = _mm512_loadu_ps(&A[k][j]);
                __m512 vaij = _mm512_loadu_ps(&A[i][j]);
                __m512 vx = _mm512_mul_ps(vakj, vaik);
                vaij = _mm512_sub_ps(vaij, vx);
                _mm512_storeu_ps(&A[i][j], vaij);
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