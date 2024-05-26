#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  // Pthread编程
#include <arm_neon.h>

using namespace std;

// 都放静态存储区，节省内存
float** A;
int n;

// 用于线程函数传参：线程号id和当前行k
typedef struct {
    int id;
    int k;
} ThreadArgs;

// 并行函数
void* thread_func(void* arg) {
    // 传参
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;
    int k = thread_arg->k;

    // 一个线程负责一行
    int i = k + id + 1;
    // 创建向量
    float32x4_t vx, vaij, vaik, vakj;
    vaik = vld1q_dup_f32(&A[i][k]);
    int j;
    // j: k ~ n-1，向量化
    for (j=k; j+4<=n; j+=4) {
        // A[i][j] -= A[i][k] * A[k][j];
        vakj = vld1q_f32(&A[k][j]);
        vaij = vld1q_f32(&A[i][j]);
        vx = vmulq_f32(vakj, vaik);
        vaij = vsubq_f32(vaij, vx);
        vst1q_f32(&A[i][j], vaij);
    }
    // 不能整除的部分
    for (; j<n; j++) {
        A[i][j] -= A[i][k] * A[k][j];
    }
    pthread_exit(NULL);
}

// Pthread动态线程
void LU() {
    for (int k=0; k<n; k++) {
        // 串行除法
        for (int i=k; i<n; i++) {
            A[k][i] /= A[k][k];
        }
        // 多线程并行减法，此方法缺点：频繁创建线程，开销大
        // 初始化线程的id
        int worker_count = n - 1 - k;
        pthread_t* threads = new pthread_t[n - 1 - k];
        ThreadArgs* thread_ids = new ThreadArgs[n - 1 - k];
        for (int i=0; i<n-1-k; i++) {
            thread_ids[i] = {i, k};
        }
        // 创建线程
        for (int i=0; i<n-1-k; i++) {
            pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        }
        // 等待线程计算完毕
        for (int i=0; i<n-1-k; i++) {
            pthread_join(threads[i], NULL);
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