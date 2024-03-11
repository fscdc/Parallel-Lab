#include <iostream>

using namespace std;

int a[10001], sum;

int main() {
    for (int i=0; i<10000; i++) {
        sum = 0;
        int n = 8192;
        for (int i=0; i<n; i++) {
            a[i] = i;
        }
        for (int i=0; i<n; i++) {
            sum += a[i];
        }
    }
    cout << sum;
    return 0;
}