#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <string.h>
#include <chrono>
#include <arm_neon.h>


// #ifndef DATA
// #define DATA "./Groebner/1_130_22_8/"
// #define COL 130
// #define ELE 22
// #define ROW 8
// #endif

// #ifndef DATA
// #define DATA "./Groebner/2_254_106_53/"
// #define COL 254
// #define ELE 106
// #define ROW 53
// #endif

// #ifndef DATA
// #define DATA "./Groebner/3_562_170_53/"
// #define COL 562
// #define ELE 170
// #define ROW 53
// #endif

// #ifndef DATA
// #define DATA "./Groebner/4_1011_539_263/"
// #define COL 1011
// #define ELE 539
// #define ROW 263
// #endif

#ifndef DATA
#define DATA "./Groebner/6_3799_2759_1953/"
#define COL 3799
#define ELE 2759
#define ROW 1953
#endif

#define mat_t unsigned int
#define mat_L 32
#define REPT 4

#define ALIGN
// #define DEBUG


using namespace std;

mat_t ele[COL][COL / mat_L + 1] = {0};
mat_t row[ROW][COL / mat_L + 1] = {0};

#ifndef ALIGN
mat_t ele_tmp[COL][COL / mat_L + 1] = {0};
mat_t row_tmp[ROW][COL / mat_L + 1] = {0};
#else
mat_t ele_tmp[COL][(COL / mat_L + 1) / 16 * 16 + 16] __attribute__((aligned(64))) = {0};
mat_t row_tmp[ROW][(COL / mat_L + 1) / 16 * 16 + 16] __attribute__((aligned(64))) = {0};
#endif

void test(void (*func)(mat_t[COL][COL / mat_L + 1], mat_t[ROW][COL / mat_L + 1]), const char *msg)
{
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < REPT; i++)
        func(ele, row);
    auto end = chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
    cout <<msg<<": "<<diff.count() <<"ms"<<endl;
}

void groebner(mat_t ele[COL][COL / mat_L + 1], mat_t row[ROW][COL / mat_L + 1])
{
    // ele=消元子，row=被消元行
    memcpy(ele_tmp, ele, sizeof(mat_t) * COL * (COL / mat_L + 1));
    memcpy(row_tmp, row, sizeof(mat_t) * ROW * (COL / mat_L + 1));
    for (int i = 0; i < ROW; i++)
    {
        for (int j = COL; j >= 0; j--)
        {
            if (row_tmp[i][j / mat_L] & ((mat_t)1 << (j % mat_L)))
            {
                if (ele_tmp[j][j / mat_L] & ((mat_t)1 << (j % mat_L)))
                {
                    for (int p = COL / mat_L; p >= 0; p--)
                        row_tmp[i][p] ^= ele_tmp[j][p];
                }
                else
                {
                    memcpy(ele_tmp[j], row_tmp[i], (COL / mat_L + 1) * sizeof(mat_t));
                    break;
                }
            }
        }
    }
#ifdef DEBUG
    for (int i = 0; i < ROW; i++)
    {
        cout << i << ": ";
        for (int j = COL; j >= 0; j--)
            if (row_tmp[i][j / mat_L] & ((mat_t)1 << (j % mat_L)))
                cout << j << ' ';
        cout << endl;
    }
#endif
}

void groebner_neon(mat_t ele[COL][COL / mat_L + 1], mat_t row[ROW][COL / mat_L + 1])
{
    // ele=消元子，row=被消元行
    memcpy(ele_tmp, ele, sizeof(mat_t) * COL * (COL / mat_L + 1));
    memcpy(row_tmp, row, sizeof(mat_t) * ROW * (COL / mat_L + 1));
    uint32x4_t row_i, ele_j;
    for (int i = 0; i < ROW; i++)
    {
        for (int j = COL; j >= 0; j--)
        {
            if (row_tmp[i][j / mat_L] & ((mat_t)1 << (j % mat_L)))
            {
                if (ele_tmp[j][j / mat_L] & ((mat_t)1 << (j % mat_L)))
                {
                    for (int p = 0; p < COL / 128; p++)
                    {
                        row_i = vld1q_u32(row_tmp[i] + p * 4);
                        ele_j = vld1q_u32(ele_tmp[j] + p * 4);
                        vst1q_u32(row_tmp[i] + p * 4, veorq_u32(row_i, ele_j));
                    }
                    for (int k = COL / 128 * 4; k <= COL / mat_L; k++)
                        row_tmp[i][k] ^= ele_tmp[j][k];
                }
                else
                {
                    memcpy(ele_tmp[j], row_tmp[i], (COL / mat_L + 1) * sizeof(mat_t));
                    break;
                }
            }
        }
    }
#ifdef DEBUG
    for (int i = 0; i < ROW; i++)
    {
        cout << i << ": ";
        for (int j = COL; j >= 0; j--)
            if (row_tmp[i][j / mat_L] & ((mat_t)1 << (j % mat_L)))
                cout << j << ' ';
        cout << endl;
    }
#endif
}


int main()
{
    // DATA INPUT
    ifstream data_ele((string)DATA + (string) "1.txt", ios::in);
    int temp, header;
    string line;
    for (int i = 0; i < ELE; i++)
    {
        getline(data_ele, line);
        istringstream line_iss(line);
        line_iss >> header;
        ele[header][header / mat_L] += (mat_t)1 << (header % mat_L);
        while (line_iss >> temp)
            ele[header][temp / mat_L] += (mat_t)1 << (temp % mat_L);
    }
    data_ele.close();

    ifstream data_row((string)DATA + (string) "2.txt", ios::in);
    for (int i = 0; i < ROW; i++)
    {
        getline(data_row, line);
        istringstream line_iss(line);
        while (line_iss >> temp)
            row[i][temp / mat_L] += (mat_t)1 << (temp % mat_L);
    }
    data_row.close();

    // TEST
#ifdef ALIGN
    cout<<"ALIGHED"<<endl;
#endif
    test(groebner, "common");
    test(groebner_neon, "neon");
    return 0;
}