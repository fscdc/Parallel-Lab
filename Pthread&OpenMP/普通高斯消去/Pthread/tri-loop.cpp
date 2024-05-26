#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  
#include <semaphore.h>  

using namespace std;

#define NUM_THREADS 4

float** A;
int n;

// Semaphore for synchronization during division and elimination
sem_t sem_leader;
sem_t sem_division[NUM_THREADS - 1];
sem_t sem_elimination[NUM_THREADS - 1];

// Structure for passing thread arguments: thread id
// Since only one thread is created, there is no need to keep track of the current row k.
// Instead, it is a "round-robin" operation between the main thread and the worker threads, where both sides increment the loop variable k together.
typedef struct {
    int id;
} ThreadArgs;


// All three loops are placed inside the thread function.
// Each thread has its own function, with different ids indicating the portion it needs to process.
void* thread_func(void* arg) {
    // Get the thread arguments
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;

    for (int k=0; k<n; k++) {
        // Thread with id 0 performs division operation
        if (id == 0) {
            for (int j=k+1; j<n; j++) {
                A[k][j] /= A[k][k];
            }
            A[k][k] = 1.0;
            // Division completed, wake up other threads
            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_post(&sem_division[i]);
            }
        } else {
            // Other threads wait
            sem_wait(&sem_division[id - 1]);
        }

        // Eliminate the kth column elements of rows [k+1, n)
        // Divide the work among NUM_THREADS threads based on row intervals
        // Thread 0 also participates to improve thread utilization
        for (int i = k+1+id; i<n; i += NUM_THREADS) {
            for (int j=k+1; j<n; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
        // Synchronize with thread 0 as the leader
        if (id == 0) {
            // Wait for other worker threads to finish elimination
            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_wait(&sem_leader);
            }
            // Notify elimination completion
            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_post(&sem_elimination[i]);
            }
        } else {
            // Notify the leader
            sem_post(&sem_leader);
            // Wait for notification from thread 0
            sem_wait(&sem_elimination[id - 1]);
        }
    }
    pthread_exit(NULL);
}


void LU() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_ids[NUM_THREADS];

    // Initialize thread ids
    for (int i=0; i<NUM_THREADS; i++) {
        thread_ids[i] = {i};
    }

    // Initialize semaphores, this is important
    sem_init(&sem_leader, 0, 0);
    for (int i=0; i<NUM_THREADS-1; i++) {
        sem_init(&sem_division[i], 0, 0);
        sem_init(&sem_elimination[i], 0, 0);
    }

    // Create threads
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    // Wait for threads to finish their work
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