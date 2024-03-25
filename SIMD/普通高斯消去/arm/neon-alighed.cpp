#include <iostream>
#include <chrono>
#include <arm_neon.h>  // Neon
#include "gendata.h"

using namespace std;

// NEON-PARALLEL-ALIGHED
void LU(float** A, int N) {
    for (int k=0; k<N; k++) {
        float32x4_t vt = vld1q_dup_f32(&A[k][k]);
        int j = k + 1;

        while (j % 4 != 0) {
            A[k][j] = A[k][j] / A[k][k];
            j++;
        }
        for (; j+4<=N; j+=4) {
            float32x4_t va = vld1q_f32(&A[k][j]); 

            // va = va / vt
            va = vdivq_f32(va, vt);

            vst1q_f32(&A[k][j], va);
        }

        for (; j<N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i=k+1; i<N; i++) {
            float32x4_t vaik = vld1q_dup_f32(&A[i][k]);
            int j = k + 1;

            while (j % 4 != 0) {
                A[i][j] = A[i][j] - A[k][j]*A[i][k];
                j++;
            }
            for (; j+4<=N; j+=4) {
                float32x4_t vakj = vld1q_f32(&A[k][j]);
                float32x4_t vaij = vld1q_f32(&A[i][j]);
                float32x4_t vx = vmulq_f32(vakj, vaik);
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&A[i][j], vaij);
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