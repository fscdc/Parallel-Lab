/*
Use a char array to implement a bitmap
5 bits compose an int
Can only analyze and debug the code, very tricky
*/

#include <bits/stdc++.h>
#include <semaphore.h>  
#include <pthread.h>  

using namespace std;

// Corresponding to three parameters of the dataset: number of matrix columns, number of non-zero elimination sub-rows, number of eliminated rows
#define num_columns 1011
#define num_elimination_rows 539
#define num_eliminated_rows 263

#define NUM_THREADS 4

const int length = ceil(num_columns / 5.0);

// Use char arrays for storage, R: elimination sub-rows, E: eliminated rows
char R[10000][length];  
char E[num_eliminated_rows][length];

// Record <row number of elimination operation, column number of the first term>
vector<pair<int, int>> records;

// Record whether lifted
bool lifted[num_eliminated_rows];

// Semaphores, synchronize division and elimination
sem_t sem_leader;
sem_t sem_division[NUM_THREADS - 1];
sem_t sem_elimination[NUM_THREADS - 1];

// Pthread
typedef struct {
    int id;
} ThreadArgs;


void print() {
    for (int i=0; i<num_eliminated_rows; i++) {
        for (int j=num_columns-1; j>=0; j--) {
            if (((E[i][j / 5] >> (j - 5*(j/5)))) & 1 == 1) {
                cout << j << ' ';
            }
        }
        cout << endl;
    }
}

// Parallel Pthread implementation of special Gaussian elimination, assuming 5 columns of elimination sub-rows/eliminated rows are taken out each round
void* thread_func(void* arg) {
    // Pass arguments
    ThreadArgs* thread_arg = (ThreadArgs*)arg;
    int id = thread_arg->id;
    // Traverse the 5 bits of each elimination sub-row and eliminated row, implemented through an element of the array
    // E[i][x] corresponds to the 5x ~ 5(x+1)-1 bits, storing bits from right to left
    // The advantage of this storage method: no need to consider boundaries
    for (int n = length - 1; n >= 0; n--) {
        // Single-threaded recording
        if (id == 0) {
            records.clear();
            // Traverse the eliminated rows
            for (int i=0; i<num_eliminated_rows; i++) {
                // Do not process the lifted rows
                if (lifted[i]) {
                    continue;
                }
                // Find the first term to eliminate, must find from high to low
                for (int j = 5*(n+1)-1; j >= 5*n; j--) {
                    if (E[i][n] >> (j-5*n) == 1) {
                        if (R[j][n] != 0) {
                            E[i][n] ^= R[j][n];
                            records.emplace_back(i, j);
                        } else {
                            // Immediately lift, facilitate the elimination of other rows later
                            for (auto pair : records) {
                                int row = pair.first;
                                int leader = pair.second;
                                if (row == i) {
                                    // Fill in the remaining bits with XOR
                                    for (int k=n-1; k>=0; k--) {
                                        E[i][k] ^= R[leader][k];
                                    }
                                }
                            }
                            // Elimination sub-row j = eliminated row i
                            for (int k=0; k<length; k++) {
                                R[j][k] = E[i][k];
                            }
                            lifted[i] = true;
                            break;
                        }
                    }
                }
            }
            // Calculation completed, wake up other threads
            for (int i=0; i<NUM_THREADS-1; i++) {
                sem_post(&sem_division[i]);
            }
        } else {
            // Other threads wait first
            sem_wait(&sem_division[id - 1]);
        }
        // Next, perform parallel calculation on the remaining columns, according to the records in the records for multi-threaded operations
        // Pthread multi-thread parallelization
        for (int i = 0; i<n; i += NUM_THREADS) {
            for (auto pair : records) {
                int row = pair.first;
                int leader = pair.second;
                // Skip lifted rows
                if (lifted[row]) {
                    continue;
                }
                E[row][i] ^= R[leader][i];
            }
        }
        // Synchronize based on thread 0
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
            // Notify leader
            sem_post(&sem_leader);
            // Wait for notification from thread 0
            sem_wait(&sem_elimination[id - 1]);
        }
    }
}

// Main thread
void solve() {
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

    // Threads finish their work
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    ifstream file_R;
    char buffer[10000] = {0};
    // file_R.open("/home/data/Groebner/测试样例1 矩阵列数130，非零消元子22，被消元行8/消元子.txt");
    file_R.open("R.txt");
    if (file_R.fail()) {
        cout << "Failed to read" << endl;
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, record each 32 bits of elimination sub-rows into an int
        int bit;
        stringstream line(buffer);
        int first_in = 1;

        // The index of the elimination sub-row is its first term
        int index;
        while (line >> bit) {
            if (first_in) {
                first_in = 0;
                index = bit;
            }

            // Set the corresponding bit of the elimination sub-row of the index row to 1
            R[index][bit / 5] |= 1 << (bit - (bit / 5) * 5);
        }
    }
    file_R.close();

    ifstream file_E;
    // file_E.open("/home/data/Groebner/测试样例1 矩阵列数130，非零消元子22，被消元行8/被消元行.txt");
    file_E.open("E.txt");

    // The index of the eliminated row is the number of lines read
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, record each 32 bits of eliminated rows into an int
        int bit;
        stringstream line(buffer);
        while (line >> bit) {
            // Set the corresponding bit of the eliminated row of the index row to 1
            E[index][bit / 5] |= (1 << (bit - (bit / 5) * 5));
        }
        index++;
    }
    // Use the chrono library in C++11 to time
    auto start = chrono::high_resolution_clock::now();
    solve();
    auto end = chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
    cout << diff.count() << "ms" << endl;
    return 0;
}