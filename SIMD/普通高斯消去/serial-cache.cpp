#include <iostream>
#include <chrono>
#include "gendata.h"

using namespace std;

float B[5000][5000];

// serial-cache
void LU(float** A, int N) {
    for (int i=0; i<N; i++) {
        for (int j=0; j<i; j++) {
            B[j][i] = A[i][j];
            A[i][j] = 0;
        }
    }
    for (int k=0; k<N; k++) {
        for (int j=k+1; j<N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i=k+1; i<N; i++) {
            for (int j=k+1; j<N; j++) {
                A[i][j] = A[i][j] - A[k][j]*B[k][i];
            }
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