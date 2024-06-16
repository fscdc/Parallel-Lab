%%writefile lab/matrix.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <sycl/sycl.hpp>

namespace s = sycl;

#define num_columns 1011
#define num_elimination_rows 539
#define num_eliminated_rows 263

const int length = 203;

// 使用char数组进行存储，R：消元子，E：被消元行
char R[10000][length]; 
char E[num_eliminated_rows][length];

bool lifted[num_eliminated_rows];

// print
void print() {
    for (int i=0; i<num_eliminated_rows; i++) {
        for (int j=num_columns-1; j>=0; j--) {
            if ((((E[i][j / 5] >> (j - 5*(j/5)))) & 1) == 1) {
                std::cout << j << ' ';
            }
        }
        std::cout << std::endl;
    }
}

// 读入
void input() {
    // 读入消元子
    std::ifstream file_R;
    char buffer[10000] = {0};
    file_R.open("/home/s2112213/parallel_FSC/GPU/R.txt");
    if (file_R.fail()) {
        std::cout << "读入失败" << std::endl;
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // 每一次读入一行，消元子每32位记录进一个int中
        int bit;
        std::stringstream line(buffer);
        int first_in = 1;

        // 消元子的index是其首项
        int index;
        while (line >> bit) {
            if (first_in) {
                first_in = 0;
                index = bit;
            }

            // 将第index行的消元子对应位 置1
            R[index][bit / 5] |= 1 << (bit - (bit / 5) * 5);
        }
    }
    file_R.close();

    // 读入被消元行
    std::ifstream file_E;
    file_R.open("/home/s2112213/parallel_FSC/GPU/E.txt");

    // 被消元行的index就是读入的行数
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // 每一次读入一行，消元子每32位记录进一个int中
        int bit;
        std::stringstream line(buffer);
        while (line >> bit) {
            // 将第index行的被消元行对应位 置1
            E[index][bit / 5] |= (1 << (bit - (bit / 5) * 5));
        }
        index++;
    }
}

// 特殊高斯消去法串行 char 数组实现
void solve() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 遍历被消元行：逐元素消去，一次清空
    for (int i = 0; i < num_eliminated_rows; i++) {
        bool is_eliminated = false;
        // 行内遍历依次找首项消去
        for (int j = length - 1; j >= 0; j--) {
            // cout << j << "消去" << endl;
            for (int k=4; k>=0; k--) {
                if ((E[i][j] >> k) == 1) {
                    // 获得首项
                    int leader = 5 * j + k;
                    if (R[leader][j] != 0) {
                        // 有消元子就消去，需要对整行异或
                        for (int m=j; m>=0; m--) {
                            E[i][m] ^= R[leader][m];
                        }
                    } else {
                        // 否则升格，升格之后这一整行都可以不用管了
                        for (int m=j; m>=0; m--) {
                            R[leader][m] = E[i][m];
                        }
                        // 跳出多重循环
                        is_eliminated = true;
                    }
                }
                if (is_eliminated) {
                    break;
                }
            }
            if (is_eliminated) {
                break;
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start);
    std::cout << diff.count() << "ms" << std::endl;
}

int main() {
    input();
    solve();
    print();
    return 0;
}
