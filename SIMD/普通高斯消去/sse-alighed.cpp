#include <iostream>
#include <chrono>
#include <nmmintrin.h>  // SSE4.2
#include "gendata.h"

using namespace std;

// SEE-PARALLEL-ALIGHED
void LU(float** A, int N) {
    for (int k=0; k<N; k++) {
        // div parallel
        __m128 vt = _mm_load1_ps(&A[k][k]); 
        int j = k + 1;

        // A[k][j] needs to be memory aligned, originally requiring (k*N + j) % 4 == 0
        // for convenience, here N is taken as a multiple of 4, so the condition becomes j % 4 == 0
        while (j % 4 != 0) {
            A[k][j] = A[k][j] / A[k][k];
            j++;
        }
        for (; j+4<=N; j+=4) {
            __m128 va = _mm_load_ps(&A[k][j]);  // load，aligned load

            va = _mm_div_ps(va, vt);

            _mm_store_ps(&A[k][j], va); // store，aligned store
        }

        for (; j<N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i=k+1; i<N; i++) {
            __m128 vaik = _mm_load1_ps(&A[i][k]);
            int j = k + 1;;

            while (j % 4 != 0) {
                A[i][j] = A[i][j] - A[k][j]*A[i][k];
                j++;
            }
            for (; j+4<=N; j+=4) {
                // origin code: A[i][j] = A[i][j] - A[k][j]*A[i][k];
                __m128 vakj = _mm_load_ps(&A[k][j]);
                __m128 vaij = _mm_load_ps(&A[i][j]);
                __m128 vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_store_ps(&A[i][j], vaij);
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