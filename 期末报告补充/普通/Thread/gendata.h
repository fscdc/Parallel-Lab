#include <iostream>
#include <random>

using namespace std;

static mt19937 gen(42);  // set 42 as random seed

// gen random float range 0 to 1
float get_random_float() {
    uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

// RANDOM MATRIX GENERATOR(TOTALLY)
void reset(float** m, int n) {
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            m[i][j] = get_random_float();
        }
    }
}

// print function
void print(float** m, int n) {
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            cout << m[i][j] << ' ';
        }
        cout << endl;
    }
}

