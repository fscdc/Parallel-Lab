#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  

using namespace std;

#define NUM_THREADS 4

float** A;
int n;

pthread_barrier_t barrier;

// Used for passing arguments to the thread function: thread id
// Since only one thread is created, there is no need to keep track of the current row k.
// Instead, it is a "turn-based" operation between the main thread and the worker thread,
// where both sides increment the loop variable k together.
typedef struct {
    int id;
} ThreadArgs;

// Each thread has its own copy of this function, with different id indicating the portion it needs to process.
// Barrier synchronization requires both division and subtraction to be written in the thread function.
void* thread_func(void* arg) {
    // Pass arguments
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;

    for (int k=0; k<n; k++) {
        // Serial division, which can be parallelized here
        for (int i = k+id; i<n; i += NUM_THREADS) {
            A[k][i] /= A[k][k];
        }
        // First synchronization: division synchronization
        pthread_barrier_wait(&barrier);
        // Eliminate the k-th column elements of rows [k+1, n)
        // Divide the work among NUM_THREADS threads based on row intervals
        for (int i = k+1+id; i<n; i += NUM_THREADS) {
            for (int j=k; j<n; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
        // Second synchronization: subtraction synchronization
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(NULL);
}

// Ordinary Gaussian elimination, LU decomposition
void LU() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_ids[NUM_THREADS];

    // Initialize the barrier for thread synchronization
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    // Initialize thread ids
    for (int i=0; i<NUM_THREADS; i++) {
        thread_ids[i] = {i};
    }
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    // Wait for threads to finish their work
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    // Destroy the barrier
    pthread_barrier_destroy(&barrier);
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