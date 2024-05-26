#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  

using namespace std;

float** A;
int n;

// Struct used for passing arguments to thread function: thread id and current row k
typedef struct {
    int id;
    int k;
} ThreadArgs;

// Parallel function
void* thread_func(void* arg) {
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;
    int k = thread_arg->k;

    int i = k + id + 1;
    for (int j=k; j<n; j++) {
        A[i][j] -= A[i][k] * A[k][j];
    }
    pthread_exit(NULL);
}

// Pthread dynamic threads
void LU() {
    for (int k=0; k<n; k++) {
        // Serial division
        for (int i=k; i<n; i++) {
            A[k][i] /= A[k][k];
        }
        // Parallel subtraction using multiple threads, drawback: frequent thread creation, high overhead
        // Initialize thread ids
        int worker_count = n - 1 - k;
        pthread_t* threads = new pthread_t[n - 1 - k];
        ThreadArgs* thread_ids = new ThreadArgs[n - 1 - k];
        for (int i=0; i<n-1-k; i++) {
            thread_ids[i] = {i, k};
        }
        // Create threads
        for (int i=0; i<n-1-k; i++) {
            pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        }
        // Wait for threads to finish computation
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