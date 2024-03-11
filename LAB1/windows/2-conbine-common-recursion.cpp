#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int a[262200], sum;

// ��ʼ��
void init(int n) {
    for (int i=0; i<n; i++) {
        a[i] = i;
    }
}

// ƽ���㷨
void common(int n) {
    // #pragma UNROLL(4)
    for (int i=0; i<n; i++) {
        sum += a[i];
    }
}

// ˫·���
void multi(int n) {
    int sum1 = 0, sum2 = 0;
    for (int i=0; i<n; i+=2) {
        sum1 += a[i];
        sum2 += a[i+1];  // n������ż��
    }
    sum = sum1 + sum2;
}

// ����ѭ��ʵ�ֵݹ�
void recursion(int n) {
    int m;
    for (m=n; m>1; m/=2) {
        for (int i=0; i < m/2; i++) {
            a[i] = a[i*2] + a[i*2 + 1];
        }
    }  // a[0]Ϊ���ս��
    sum = a[0];
}

// �ݹ麯��ʵ�ֵݹ�
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
    int n = 2;  // ��ģ
    while (n < 262200) {
        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        int counter = 0;
        while (counter < 10000) {  // �ظ�10000�Σ�����ʱ��
            init(n);
            sum = 0;
            counter++;
            common(n);
        }
        // ��ʱ����
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout <<  "��ģ" << n << ':';
        cout << "ƽ���㷨" << ' ' << ms / counter << ' ';

        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            multi(n);
        }
        // ��ʱ����
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "˫·�Ż�����㷨" << ' ' << ms / counter << ' ';

        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion2(n);
        }
        // ��ʱ����
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "�ݹ��Ż��㷨���ݹ麯��ʵ�֣�" << ' ' << ms / counter << ' ';


        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        counter = 0;
        while (counter < 10000) {
            init(n);
            sum = 0;
            counter++;
            recursion(n);
        }
        // ��ʱ����
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        ms = (tail-head) * 1000 / freq;
        cout << "�ݹ��Ż��㷨��˫��ѭ��ʵ�֣�" << ' ' << ms / counter << endl;

        n *= 2;
    }
}