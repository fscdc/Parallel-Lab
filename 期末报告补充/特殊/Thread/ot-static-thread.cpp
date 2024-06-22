#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>


// Parameters for the dataset: number of columns in the matrix, number of elimination rows, number of eliminated rows
#define num_columns 1011
#define num_elimination_rows 539
#define num_eliminated_rows 263

// Length of the arrays
const int length = 203;

// Use char arrays for storage, R: elimination rows, E: eliminated rows
char R[10000][length];  // R[i] stores the elimination row with the leading term i (index starts from 0)
                        // So the array size cannot be directly set to num_elimination_rows
char E[num_eliminated_rows][length];

// Record whether a row is lifted
bool lifted[num_eliminated_rows];

// Print the current bitmap to the screen
void print() {
    for (int i = 0; i < num_eliminated_rows; i++) {
        for (int j = num_columns - 1; j >= 0; j--) {
            // If the j-th bit is 1
            if ((((E[i][j / 5] >> (j - 5 * (j / 5)))) & 1) == 1) {
                std::cout << j << ' ';
            }
        }
        std::cout << std::endl;
    }
}

// Read input
void input() {
    // Read elimination rows
    std::ifstream file_R;
    char buffer[10000] = {0};
    file_R.open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例4 矩阵列数1011，非零消元子539，被消元行263/消元子.txt");
    if (file_R.fail()) {
        std::cout << "Failed to open file" << std::endl;
        perror("File error");
        std::string command = "dir \"D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例4 矩阵列数1011，非零消元子539，被消元行263/消元子.txt\"";
        int result = std::system(command.c_str());
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, each 32 bits of the elimination row are stored in an int
        int bit;
        std::stringstream line(buffer);
        int first_in = 1;

        // The index of the elimination row is its leading term
        int index;
        while (line >> bit) {
            if (first_in) {
                first_in = 0;
                index = bit;
            }

            // Set the corresponding bit of the elimination row at index to 1
            R[index][bit / 5] |= 1 << (bit - (bit / 5) * 5);
        }
    }
    file_R.close();

    // Read eliminated rows
    std::ifstream file_E;
    file_E.open("D:/study/vscode/parallel/Parallel_Final_Research/特殊高斯/Groebner/测试样例4 矩阵列数1011，非零消元子539，被消元行263/被消元行.txt");

    // The index of the eliminated row is the line number read
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, each 32 bits of the eliminated row are stored in an int
        int bit;
        std::stringstream line(buffer);
        while (line >> bit) {
            // Set the corresponding bit of the eliminated row at index to 1
            E[index][bit / 5] |= (1 << (bit - (bit / 5) * 5));
        }
        index++;
    }
}

// Parallel implementation of special Gaussian elimination using char arrays
void solve() {
    auto start = std::chrono::high_resolution_clock::now();

    const int num_threads = std::thread::hardware_concurrency();  // Get the number of available threads
    std::vector<std::thread> threads(num_threads);

    // Traverse the eliminated rows: eliminate one element at a time, clear once
    for (int i = 0; i < num_eliminated_rows; i++) {
        bool is_eliminated = false;
        // Traverse within the row to find the leading term to eliminate
        for (int j = length - 1; j >= 0; j--) {
            // cout << j << "eliminated" << endl;
            for (int k = 4; k >= 0; k--) {
                if ((E[i][j] >> k) == 1) {
                    // Get the leading term
                    int leader = 5 * j + k;
                    if (R[leader][j] != 0) {
                        // If there is an elimination row, eliminate it, and perform XOR on the entire row in parallel
                        for (int t = 0; t < num_threads; t++) {
                            threads[t] = std::thread([&](int thread_id, int start, int end) {
                                for (int m = start; m >= end; m--) {
                                    E[i][m] ^= R[leader][m];
                                }
                            }, t, j, 0);
                        }

                        for (int t = 0; t < num_threads; t++) {
                            threads[t].join();
                        }
                    } else {
                        // Otherwise, lift the row, and after lifting, the entire row can be ignored
                        for (int m = j; m >= 0; m--) {
                            R[leader][m] = E[i][m];
                        }
                        // Break out of multiple loops
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
    return 0;
}
