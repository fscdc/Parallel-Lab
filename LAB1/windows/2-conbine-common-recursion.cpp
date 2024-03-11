#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int a[262200], sum;

// 初始化
void init(int n) {
    for (int i=0; i<n; i++) {
        a[i] = i;
    }
}

// 平凡算法
void common(int n) {
    // #pragma UNROLL(4)
    for (int i=0; i<n; i++) {
        sum += a[i];
    }
}

// 双路求和
void multi(int n) {
    int sum1 = 0, sum2 = 0;
    for (int i=0; i<n; i+=2) {
        sum1 += a[i];
        sum2 += a[i+1];  // n必须是偶数
    }
    sum = sum1 + sum2;
}

// 二重循环实现递归
void recursion(int n) {
    int m;
    for (m=n; m>1; m/=2) {
        for (int i=0; i < m/2; i++) {
            a[i] = a[i*2] + a[i*2 + 1];
        }
    }  // a[0]为最终结果
    sum = a[0];
}

// 递归函数实现递归
void recursion2(int n) {
    if (n == 1) {
        return;
    } else {
        for (int i=0; i < n/2; i++) {
            a[i] += a[n-i-1];
        }
        n /= 2;
        recursion2(n);
    }
    sum = a[0];
    return;
}

int main() {
    long long head, tail, freq;
    float ms;
    int n = 2;  // 规模
    while (n < 262200) {
        // 计时开始
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        int counter = 0;
        while (counter < 10000) {  // 重复10000次，增大时间
            init(n);
            sum = 0;
            counter++;
            common(n);
        }
        // 计时结束
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout <<  "规模" << n << ':';
        cout << "平凡算法" << ' ' << ms / counter << ' ';

        // 计时开始
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            multi(n);
        }
        // 计时结束
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "双路优化求和算法" << ' ' << ms / counter << ' ';

        // 计时开始
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion2(n);
        }
        // 计时结束
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "递归优化算法（递归函数实现）" << ' ' << ms / counter << ' ';


        // 计时开始
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion(n);
        }
        // 计时结束
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "递归优化算法（双重循环实现）" << ' ' << ms / counter << endl;

        n *= 2;
    }
}