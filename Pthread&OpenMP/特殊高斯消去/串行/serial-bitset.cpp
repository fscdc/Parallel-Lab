#include <bits/stdc++.h>

using namespace std;

// Corresponding to three parameters of the dataset: number of columns in the matrix, number of elimination sub-rows, number of eliminated rows
#define num_columns 254
#define num_elimination_rows 106
#define num_eliminated_rows 53

// Using bitset for storage, R: elimination sub-rows, E: eliminated rows
bitset<num_columns> R[10000];  // R[i] records the elimination sub-row with the leading term i (index starts from 0)
                               // So the array size cannot be directly set to num_elimination_rows
bitset<num_columns> E[num_eliminated_rows];

// Serial implementation of special Gaussian elimination
void solve() {
    // Loop through each eliminated row
    for (int i = 0; i < num_eliminated_rows; i++) {
        // If the current eliminated row is zero, skip it
        if (E[i].none()) {
            continue;
        }

        // Loop through each term in the current eliminated row
        while (!E[i].none()) {
            // Find the leading term of the current eliminated row
            int k = num_columns - 1;
            while (E[i][k]==0 && k>=0) {
                k--;
            }
            // cout << "Leading term" << k;

            // If the elimination sub-row corresponding to the leading term does not exist, "promote" it by adding the current row to the set of this elimination sub-row
            if (!R[k].any()) {
                R[k] = E[i];
                // E[i].reset();
                // cout << "Promote";
                break;
            }
            // If the elimination sub-row corresponding to the leading term exists, perform elimination
            else {
                E[i] ^= R[k];
            }
        }
    }
}

void print() {
    for (int i=0; i<num_eliminated_rows; i++) {
        // cout << i << ':';
        for (int j=num_columns-1; j>=0; j--) {
            if (E[i][j] == 1) {
                cout << j << ' ';
            }
        }
        cout << endl;
    }
}

int main() {
    // Read in the elimination sub-rows
    ifstream file_R;
    char buffer[10000] = {0};
    // file_R.open("/home/data/Groebner/测试样例1 矩阵列数130，非零消元子22，被消元行8/消元子.txt");
    file_R.open("R.txt");
    if (file_R.fail()) {
        cout << "Read failed" << endl;
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // Read in one line at a time, each 32 bits of the elimination sub-row are recorded in an int
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

            // Set the corresponding bit of the elimination sub-row bitset of the index row to 1
            R[index][bit] = 1;
        }
    }
    file_R.close();
//--------------------------------
    // Read in the eliminated rows
    ifstream file_E;
    // file_E.open("/home/data/Groebner/测试样例1 矩阵列数130，非零消元子22，被消元行8/被消元行.txt");
    file_E.open("E.txt");

    // The index of the eliminated row is the number of lines read in
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // Read in one line at a time, each 32 bits of the elimination sub-row are recorded in an int
        int bit;
        stringstream line(buffer);
        while (line >> bit) {
            // Set the corresponding bit of the elimination sub-row bitset of the index row to 1
            E[index][bit] = 1;
        }
        index++;
    }
//--------------------------------
    // Use the chrono library in C++11 to measure time
    auto start = chrono::high_resolution_clock::now();
    solve();
    auto end = chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
    cout << diff.count() << "ms" << endl;
//--------------------------------
    // Verify the correctness of the result
    // print();
    return 0;
}