#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int N = 10001;

double b[N][N], sum[N], a[N];

void init(int n) {  
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            b[i][j] = i + j;
    for (int i = 0; i < n; i++) {
        a[i] = i;
    }
}

// 普通算法函数
void common(int n) {
    for (int i = 0; i < n; i++) {
        sum[i] = 0.0;
        for (int j = 0; j < n; j++)
            sum[i] += b[j][i] * a[i];
    }
}

// Cache优化算法函数
void cache(int n) {
    for (int i = 0; i < n; i++) {
        sum[i] = 0;
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++)
            sum[i] += b[j][i] * a[i];
    }
}

int main() {
    int step = 100;
    long counter;
    float us;
    struct timeval start, end;

    for (int n = 0; n <= 10000; n += step) {
        init(n);
        counter = 0;

        // 计时开始 - 普通算法
        gettimeofday(&start, NULL);
        while (counter < 10) {
            counter++;
            common(n);  // 调用普通算法函数
        }
        gettimeofday(&end, NULL);
        // 计时结束
        us = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec; 
        cout << "规模" << n << ':';
        cout << "平凡算法" << ' ' << us / counter / 1000 << ' ';

        counter = 0;

        // 计时开始 - Cache优化算法
        gettimeofday(&start, NULL);
        while (counter < 10) {
            counter++;
            cache(n);  // 调用Cache优化算法函数
        }
        gettimeofday(&end, NULL);
        us = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        cout << "Cache优化算法" << ' ' << us / counter / 1000 << endl;
        
        if (n == 1000) step = 1000;
    }

    return 0;
}
