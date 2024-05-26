/*
Use an integer array to implement a bitmap
5 bits compose an int
Can only analyze and debug the code, very tricky
*/

#include <bits/stdc++.h>

using namespace std;

// Corresponding to three parameters of the dataset: number of columns in the matrix, number of elimination sub-rows, number of eliminated rows
#define num_columns 1011
#define num_elimination_rows 539
#define num_eliminated_rows 263

const int length = ceil(num_columns / 5.0);

// Use char array for storage, R: elimination sub-row, E: eliminated row
char R[10000][length];
char E[num_eliminated_rows][length];

bool lifted[num_eliminated_rows];

// print bitmap
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

// Serial implementation of special Gaussian elimination using int array
void solve() {
    // Traverse the eliminated rows: eliminate element by element, clear once
    for (int i = 0; i < num_eliminated_rows; i++) {
        bool is_eliminated = false;
        // Traverse within the row to find the leading term to eliminate
        for (int j = length - 1; j >= 0; j--) {
            // cout << j << "eliminated" << endl;
            for (int k=4; k>=0; k--) {
                if ((E[i][j] >> k) == 1) {
                    // Get the leading term
                    int leader = 5 * j + k;
                    if (R[leader][j] != 0) {
                        // If there is an elimination sub-row, eliminate it, need to XOR the entire row
                        for (int m=j; m>=0; m--) {
                            E[i][m] ^= R[leader][m];
                        }
                    } else {
                        // Otherwise, lift it, after lifting, this entire row can be ignored
                        for (int m=j; m>=0; m--) {
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
}

int main() {
    ifstream file_R;
    char buffer[10000] = {0};
    file_R.open("R.txt");
    if (file_R.fail()) {
        cout << "Failed to read" << endl;
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, record each 32 bits of the elimination sub-row into an int
        int bit;
        stringstream line(buffer);
        int first_in = 1;

        // The index of the elimination sub-row is its leading term
        int index;
        while (line >> bit) {
            if (first_in) {
                first_in = 0;
                index = bit;
            }

            R[index][bit / 5] |= 1 << (bit - (bit / 5) * 5);
        }
    }
    file_R.close();
    ifstream file_E;
    file_E.open("E.txt");

    // The index of the eliminated row is the number of lines read
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // Read one line at a time, record each 32 bits of the eliminated row into an int
        int bit;
        stringstream line(buffer);
        while (line >> bit) {
            // Set the corresponding bit of the index-th row of the eliminated row to 1
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