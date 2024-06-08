#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "gendata.h"

using namespace std;

void GaussianEliminationMPI(float** A, int n, int myid, int num) {
    // 获取前后进程的 id
    int preid = (myid + (num - 1)) % num;
    int nextid = (myid + 1) % num;
    for (int k = 0; k < n; k++) {
        if (k % num == myid) {
            for (int j = k + 1; j < n; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;

            // HIGHLIGHT: pipeline
            // 只向下一个线程发送消息
            MPI_Send(A[k], n, MPI_FLOAT, nextid, 0, MPI_COMM_WORLD);
        } else {
            // 接收前一个线程传来的消息
            MPI_Recv(A[k], n, MPI_FLOAT, preid, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // 依次转发，直到全部传达
            if (k % num != nextid) {
                MPI_Send(A[k], n, MPI_FLOAT, nextid, 0, MPI_COMM_WORLD);
            }
        }

        for (int i = k + 1; i < n; i++) {
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
