#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "gendata.h"

using namespace std;

void GaussianEliminationMPI(float** A, int n, int myid, int num) {
    for (int k = 0; k < n; k++) {
        // HIGHLIGHT: circle partition
        if (k % num == myid) {
            for (int j = k + 1; j < n; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;

            for (int j = 0; j < num; j++) {
                if (j != myid) {
                    MPI_Send(A[k], n, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
                }
            }
        } else {
            MPI_Recv(A[k], n, MPI_FLOAT, k % num, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (int i = k + 1; i < n; i++) {
            // HIGHLIGHT: circle partition
            if (i % num == myid) {
                for (int j = k + 1; j < n; j++) {
                    A[i][j] -= A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
    }
}

int main(int argc, char** argv) {
    int myid, numprocs;
    vector<int> problemSizes = {200, 500, 1000, 2000, 3000};
    MPI_Init(&argc, &argv);
    for (int i = 0; i < 5; i++) {
        int n = problemSizes[i];  // 系数矩阵的大小

        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &myid);

        float** A = new float*[n];
        for (int j = 0; j < n; j++) {
            A[j] = new float[n];
        }
        reset(A, n);

        auto start = chrono::high_resolution_clock::now();
        GaussianEliminationMPI(A, n, myid, numprocs);
        auto end = chrono::high_resolution_clock::now();
        auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
        if (myid == 0) {
            cout << "Size = " << n << ": " << diff.count() << "ms" << endl;
        }
        // release memory
        for (int j = 0; j < n; j++) {
            delete[] A[j];
        }
        delete[] A;
    }
    MPI_Finalize();
    return 0;
}
