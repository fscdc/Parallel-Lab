#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "gendata.h"

using namespace std;

// Data block type, representing a portion of the matrix to be processed
struct DataBlock {
    float** A;
    int n;
    int start;
    int end;
};

// Task queue class
class TaskQueue {
private:
    queue<DataBlock> tasks;
    mutex mtx;
    condition_variable cv;

public:
    void enqueue(const DataBlock& task) {
        lock_guard<mutex> lock(mtx);
        tasks.push(task);
        cv.notify_one();
    }

    DataBlock dequeue() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !tasks.empty(); });
        DataBlock task = tasks.front();
        tasks.pop();
        return task;
    }
};

// Thread function for parallel task execution
void workerFunction(TaskQueue& taskQueue) {
    while (true) {
        DataBlock task = taskQueue.dequeue();

        // Process task
        for (int k = task.start; k < task.end; k++) {
            for (int j = k + 1; j < task.n; j++) {
                task.A[k][j] /= task.A[k][k];
            }
            task.A[k][k] = 1.0;
            for (int i = k + 1; i < task.n; i++) {
                for (int j = k + 1; j < task.n; j++) {
                    task.A[i][j] -= task.A[k][j] * task.A[i][k];
                }
                task.A[i][k] = 0;
            }
        }
    }
}

int main() {
    int size[] = { 200, 500, 1000, 2000, 3000 };
    for (int i = 0; i < 5; i++) {
        int n = size[i];

        // Initialize the 2D array and generate random numbers
        float** A = new float* [n];
        for (int i = 0; i < n; i++) {
            A[i] = new float[n];
        }

        // Reset the array
        reset(A, n);

        // Use the chrono library from C++11 for timing
        auto start = chrono::high_resolution_clock::now();

        const int numThreads = thread::hardware_concurrency();
        vector<thread> threads;
        TaskQueue taskQueue;

        // Create the thread pool and start the threads
        for (int t = 0; t < numThreads; t++) {
            threads.emplace_back(workerFunction, ref(taskQueue));
        }

        // Divide the tasks into blocks and enqueue them
        int chunkSize = n / numThreads;
        for (int t = 0; t < numThreads; t++) {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? n : (start + chunkSize);
            taskQueue.enqueue({A, n, start, end});
        }

        // Wait for all threads to complete the tasks
        for (auto& t : threads) {
            t.join();
        }

        auto end = chrono::high_resolution_clock::now();
        auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
        cout << "Size = " << n << ": " << diff.count() << "ms" << endl;

        // Free the memory of the 2D array A
        for (int i = 0; i < n; i++) {
            delete[] A[i];
        }
        delete[] A;
    }
    return 0;
}
