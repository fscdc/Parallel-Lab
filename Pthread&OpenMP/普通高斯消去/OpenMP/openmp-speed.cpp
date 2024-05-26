#include <bits/stdc++.h>  // 使用万能头文件stdc++.h，注意，有的IDE可能不支持
#include "gendata.h"
#include <omp.h>

using namespace std;

#define NUM_THREADS 4
#define NUM_TEAMS 4

// 都放静态存储区，节省内存
float** A;
int n;

// OpenMP高斯消去，LU分解
void LU() {
    // 循环外创建线程，避免线程反复创建销毁，影响程序性能
    float tmp;
    int i, j, k;
    bool parallel = true;
    #pragma omp teams distribute parallel for if(parallel) num_teams(NUM_TEAMS) thread_limit(NUM_THREADS) private(i, j, k, tmp)
    for (k=0; k<n; k++) {
        // 串行除法
        #pragma omp single
        {
            tmp = A[k][k];
            for (j=k; j<n; j++) {
                A[k][j] /= tmp;
            }
        }

        #pragma omp target teams distribute parallel for simd
        for (i=k+1; i<n; i++) {
            tmp = A[i][k];
            for (j=k; j<n; j++) {
                A[i][j] -= A[k][j] * tmp;
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