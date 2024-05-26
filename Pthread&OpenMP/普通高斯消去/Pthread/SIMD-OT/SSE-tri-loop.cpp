#include <bits/stdc++.h>
#include "gendata.h"
#include <pthread.h>  
#include <semaphore.h>  
#include <nmmintrin.h>

using namespace std;

#define NUM_THREADS 4

float** A;
int n;


sem_t sem_leader;
sem_t sem_division[NUM_THREADS - 1];
sem_t sem_elimination[NUM_THREADS - 1];


typedef struct {
    int id;
} ThreadArgs;


void* thread_func(void* arg) {
    __m128 vx, vaij, vaik, vakj;
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;

    for (int k=0; k<n; k++) {
        if (id == 0) {
            for (int j=k+1; j<n; j++) {
                A[k][j] /= A[k][k];
            }
            A[k][k] = 1.0;

            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_post(&sem_division[i]);
            }
        } else {

            sem_wait(&sem_division[id - 1]);
        }


        for (int i = k+1+id; i<n; i += NUM_THREADS) {

            vaik = _mm_load1_ps(&A[i][k]);
            int j;
            for (j=k+1; j+4<=n; j+=4) {
                vakj = _mm_loadu_ps(&A[k][j]);
                vaij = _mm_loadu_ps(&A[i][j]);
                vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_storeu_ps(&A[i][j], vaij);
            }

            for (; j<n; j++) {
                A[i][j] = A[i][j] - A[k][j]*A[i][k];
            }
            A[i][k] = 0;
        }

        if (id == 0) {

            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_wait(&sem_leader);
            }

            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_post(&sem_elimination[i]);
            }
        } else {
            sem_post(&sem_leader);
            sem_wait(&sem_elimination[id - 1]);
        }
    }
    pthread_exit(NULL);
}

void LU() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_ids[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++) {
        thread_ids[i] = {i};
    }

    sem_init(&sem_leader, 0, 0);
    for (int i=0; i<NUM_THREADS-1; i++) {
        sem_init(&sem_division[i], 0, 0);
        sem_init(&sem_elimination[i], 0, 0);
    }

    for (int i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

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