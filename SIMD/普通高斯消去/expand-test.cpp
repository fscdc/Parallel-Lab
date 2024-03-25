#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <immintrin.h>
#include <chrono>
#include "gendata.h"

#define N 1024
#define REPT 1
#define ele_t float


using namespace std;

ele_t new_mat[N][N] __attribute__((aligned(64)));
ele_t mat[N][N];

void test(void (*func)(ele_t[N][N], int), const char *msg, ele_t mat[N][N], int len)
{
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < REPT; i++)
        func(mat, len);
    auto end = chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
    cout <<diff.count() <<"ms"<<endl;
}

void LU(ele_t mat[N][N], int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            new_mat[i][j] = mat[i][j];

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
        {
            if (new_mat[i][i] == 0)
                continue;
            ele_t div = new_mat[j][i] / new_mat[i][i];
            for (int k = i; k < n; k++)
                new_mat[j][k] -= new_mat[i][k] * div;
        }
}

void LU_sse(float mat[N][N], int n)
{
    float new_mat[N][N];
    // 复制矩阵
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            new_mat[i][j] = mat[i][j];

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
        {
            if (new_mat[i][i] == 0)
                continue;
            float div = new_mat[j][i] / new_mat[i][i];
            __m128 div4 = _mm_set1_ps(div);
            for (int k = i; k < n; k += 4)
            {
                __m128 mat_j = _mm_loadu_ps(&new_mat[j][k]);
                __m128 mat_i = _mm_loadu_ps(&new_mat[i][k]);
                __m128 res = _mm_sub_ps(mat_j, _mm_mul_ps(div4, mat_i));
                _mm_storeu_ps(&new_mat[j][k], res);
            }
        }
}

void LU_sse_fma(float mat[N][N], int n)
{
    float new_mat[N][N];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            new_mat[i][j] = mat[i][j];

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
        {
            if (new_mat[i][i] == 0)
                continue;
            float div = new_mat[j][i] / new_mat[i][i];
            __m128 div4 = _mm_set1_ps(div);
            for (int k = i; k < n; k += 4)
            {
                __m128 mat_j = _mm_loadu_ps(&new_mat[j][k]);
                __m128 mat_i = _mm_loadu_ps(&new_mat[i][k]);
                __m128 res = _mm_fnmadd_ps(mat_i, div4, mat_j); // FMA instruction
                _mm_storeu_ps(&new_mat[j][k], res);
            }
        }
}


int main() {
    ifstream data("gauss.dat", ios::in | ios::binary);
    data.read((char *)mat, N * N * sizeof(ele_t));
    data.close();
    cout << N << ',';

    test(LU, "Serial: ", mat, N);
    test(LU_sse, "SSE: ", mat, N);
    test(LU_sse_fma, "SSE FMA: ", mat, N);
    
    return 0;
}