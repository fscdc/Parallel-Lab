#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

using namespace std;

const int N = 10001; 

double b[N][N] , sum[N], a[N];

// ��ʼ��
void init(int n) {  
    for (int i=0; i<n; i++)
        for (int j=0; j<n; j++)
            b[i][j] = i + j;
    for (int i=0; i<n; i++) {
        a[i] = i;
    }
}

int main() {
    int step = 100;
    long counter;
    float ms;
    long long head, tail, freq;
    for (int n=0; n<=10000; n+=step) {//ͨ����ģ�����ķ�ʽ������ϵͳ����������Ӱ��
        init(n);
        counter = 0;
        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        
        while (counter < 10) {  //�ظ�ʵ���С���
            counter++;
            for (int i=0; i<n; i++) {
                sum[i] = 0.0;
                for (int j=0; j<n; j++)
                    sum[i] += b[j][i] * a[i];
            }
        }
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        // ��ʱ����
        ms = (tail-head) * 1000 / freq;
        cout << "��ģ" << n << ":ƽ���㷨" << ' ' << ms / counter << ' ';

        counter = 0;
        // ��ʱ��ʼ
        QueryPerformanceFrequency((LARGE_INTEGER *)& freq);
        QueryPerformanceCounter((LARGE_INTEGER *)& head);
        
        while (counter < 10) {
            counter++;
            for (int i=0; i<n; i++) {
                sum[i] = 0;
            }
            for (int j=0; j<n; j++) {
                for (int i=0; i<n; i++)
                    sum[i] += b[j][i] * a[i];
            }
        }
        QueryPerformanceCounter((LARGE_INTEGER *)& tail);
        // ��ʱ����
        ms = (tail-head) * 1000 / freq;
        cout << "Cache�Ż��㷨" << ' ' << ms / counter << endl;
        if (n == 1000) step = 1000;
    }
}