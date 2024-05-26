#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  
#include <semaphore.h>  

using namespace std;

#define NUM_THREADS 4

float** A;
int n;

// Semaphore, each thread has two semaphores to record the start and end of this round of work
sem_t sem_start[NUM_THREADS], sem_end[NUM_THREADS];

// Used for passing arguments to the thread function: thread ID
// Since only one thread is created, there is no need to keep track of the current row k.
// Instead, it is a "turn-based" operation between the main thread and the worker threads.
// The loop variables k on both sides change together.
typedef struct {
    int id;
} ThreadArgs;

// Each thread has its own copy of this function, only the ID is different, and the part to be processed is different.
void* thread_func(void* arg) {
    // Pass the arguments
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;

    for (int k=0; k<n; k++) {
        sem_wait(&sem_start[id]);
        // Eliminate the kth column elements of rows [k+1, n)
        // Divide the rows into intervals and assign them to NUM_THREADS threads for processing.
        for (int i = k+1+id; i<n; i += NUM_THREADS) {
            for (int j=k; j<n; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
        // Notify the main thread that this thread has finished.
        sem_post(&sem_end[id]);
    }
    pthread_exit(NULL);
}

void LU() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_ids[NUM_THREADS];

    // Initialize the semaphores
    for (int i=0; i<NUM_THREADS; i++) {
        sem_init(&sem_start[i], 0, 0);
        sem_init(&sem_end[i], 0, 0);
    }

    // Create threads outside the loop first
    // Initialize the thread IDs
    for (int i=0; i<NUM_THREADS; i++) {
        thread_ids[i] = {i};
    }
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int k=0; k<n; k++) {
        // Serial division
        for (int i=k; i<n; i++) {
            A[k][i] /= A[k][k];
        }
        // Wake up the worker threads
        for (int i=0; i<NUM_THREADS; i++) {
            sem_post(&sem_start[i]);
        }
        // Main thread waits for worker threads to perform parallel subtraction
        for (int i=0; i<NUM_THREADS; i++) {
            sem_wait(&sem_end[i]);
        }
    }
    
    // Thread work is finished
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
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