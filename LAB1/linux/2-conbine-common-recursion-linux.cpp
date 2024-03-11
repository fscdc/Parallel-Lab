#include <iostream>
#include <sys/time.h>
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

// 多路链式
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
    int n = 2;  // 规模
    float us;
    struct timeval start, end;  // 计时
    while (n < 262200) {
        // 计时开始
        gettimeofday(&start, NULL);
        int counter = 0;
        while (counter < 10000) {  // 重复10000次，增大时间
            init(n);
            sum = 0;
            counter++;
            common(n);
        }
        // 计时结束
        gettimeofday(&end, NULL);
        us = (end.tv_sec-start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        cout <<  "规模" << n << ':';
        cout << "平凡算法" << ' ' << us / counter / 1000 << ' ';

        // 计时开始
        gettimeofday(&start, NULL);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            multi(n);
        }
        // 计时结束
        gettimeofday(&end, NULL);
        us = (end.tv_sec-start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        cout << "双路优化求和算法" << ' ' << us / counter / 1000 << ' ';


        // 计时开始
        gettimeofday(&start, NULL);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion2(n);
        }
        // 计时结束
        gettimeofday(&end, NULL);
        us = (end.tv_sec-start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        cout << "递归优化算法（递归函数实现）" << ' ' << us / counter / 1000 << ' ';



        // 计时开始
        gettimeofday(&start, NULL);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion(n);
        }
        // 计时结束
        gettimeofday(&end, NULL);
        us = (end.tv_sec-start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        cout << "递归优化算法（双重循环实现）" << ' ' << us / counter / 1000 << endl;

        n *= 2;
    }
}