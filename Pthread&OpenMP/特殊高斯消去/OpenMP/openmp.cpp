/*
ʹ��char����ʵ��λͼ
5��bit���һ��int
������ֻ�ܷ���������ԣ��ǳ�����
*/

#include <bits/stdc++.h>

using namespace std;

// ��Ӧ�����ݼ���������������������������Ԫ������������Ԫ������
#define num_columns 1011
#define num_elimination_rows 539
#define num_eliminated_rows 263

// �߳���
#define NUM_THREADS 4

// ���鳤��
const int length = ceil(num_columns / 5.0);

// ʹ��char������д洢��R����Ԫ�ӣ�E������Ԫ��
char R[10000][length];  // R[i]��¼������Ϊi���±��0��ʼ��¼������Ԫ����
                        // ���Բ���ֱ����num_elimination_rows���������С
char E[num_eliminated_rows][length];

// ��¼ <��Ԫ�в������кţ��������ڵ��к�>
vector<pair<int, int>> records;

// ��¼�Ƿ�����
bool lifted[num_eliminated_rows];

// ����ǰλͼ��ӡ����Ļ��
void print() {
    for (int i=0; i<num_eliminated_rows; i++) {
        // cout << i << ':';
        for (int j=num_columns-1; j>=0; j--) {
            // ��jλΪ1
            if (((E[i][j / 5] >> (j - 5*(j/5)))) & 1 == 1) {
                cout << j << ' ';
            }
        }
        // for (int j=length-1; j>=0; j--) {
        //     cout << E[i][j] << ' ';
        // }
        cout << endl;
    }
}

// �����˹��ȥ������OpenMPʵ�֣�����ÿһ��ȡ5����Ԫ��/����Ԫ�г���
void solve() {
    // cout << "in";
    int n;
    // ÿһ�α�����Ԫ�ӡ�����Ԫ�е�5��bit��ͨ�������һ��Ԫ����ʵ��
    // E[i][x]��Ӧ��5x ~ 5(x+1)-1��5��bit����������洢bit
    // �����洢�ĺô������ÿ��Ǳ߽�
    // OpenMP���̲߳��л���ѭ���ⴴ��
    #pragma omp parallel num_threads(NUM_THREADS), shared(records), private(n)  // ���private(n)����Ҫ
    for (n = length - 1; n >= 0; n--) {
        #pragma omp single
        {
            records.clear();
            // ��������Ԫ��
            for (int i=0; i<num_eliminated_rows; i++) {
                // �������������Щ��
                if (lifted[i]) {
                    continue;
                }
                // ��������ȥ������Ӹߵ�����
                for (int j = 5*(n+1)-1; j >= 5*n; j--) {
                    if (E[i][n] >> (j-5*n) == 1) {
                        if (R[j][n] != 0) {
                            E[i][n] ^= R[j][n];
                            records.emplace_back(i, j);
                        } else {
                            // �������񣬷���֮����������ȥ
                            for (auto pair : records) {
                                int row = pair.first;
                                int leader = pair.second;
                                if (row == i) {
                                    // ����ʣ��λ�����
                                    for (int k=n-1; k>=0; k--) {
                                        E[i][k] ^= R[leader][k];
                                    }
                                }
                            }
                            // ��Ԫ�ӵ�j�� = ����Ԫ�е�i��
                            for (int k=0; k<length; k++) {
                                R[j][k] = E[i][k];
                            }
                            lifted[i] = true;
                            break;
                        }
                    }
                }
            }
        }
        // ����������ʣ�µ��н��в��м��㣬����records�еļ�¼���ж��̲߳���
        // ��Щ����Ԫ�ؾͰ��ռ�¼������������Ѿ��м�¼�ˣ��������������Կ��Բ��л�
        #pragma omp for simd schedule(guided, 1)
        for (int m=n-1; m>=0; m--) {
            for (auto pair : records) {
                int row = pair.first;
                int leader = pair.second;
                // �����Ѿ��������
                if (lifted[row]) {
                    continue;
                }
                E[row][m] ^= R[leader][m];
            }
        }
        // cout << "���Կ�ʼ" << endl;
        // cout << "n=" << n << endl;
        // print();
    }
}

int main() {
    // ������Ԫ��
    ifstream file_R;
    char buffer[10000] = {0};
    // file_R.open("/home/data/Groebner/��������1 ��������130��������Ԫ��22������Ԫ��8/��Ԫ��.txt");
    file_R.open("R.txt");
    if (file_R.fail()) {
        cout << "����ʧ��" << endl;
    }
    while (file_R.getline(buffer, sizeof(buffer))) {
        // ÿһ�ζ���һ�У���Ԫ��ÿ32λ��¼��һ��int��
        int bit;
        stringstream line(buffer);
        int first_in = 1;

        // ��Ԫ�ӵ�index��������
        int index;
        while (line >> bit) {
            if (first_in) {
                first_in = 0;
                index = bit;
            }

            // ����index�е���Ԫ�Ӷ�Ӧλ ��1
            R[index][bit / 5] |= 1 << (bit - (bit / 5) * 5);
        }
    }
    file_R.close();
//--------------------------------
    // ���뱻��Ԫ��
    ifstream file_E;
    // file_E.open("/home/data/Groebner/��������1 ��������130��������Ԫ��22������Ԫ��8/����Ԫ��.txt");
    file_E.open("E.txt");

    // ����Ԫ�е�index���Ƕ��������
    int index = 0;
    while (file_E.getline(buffer, sizeof(buffer))) {
        // ÿһ�ζ���һ�У���Ԫ��ÿ32λ��¼��һ��int��
        int bit;
        stringstream line(buffer);
        while (line >> bit) {
            // ����index�еı���Ԫ�ж�Ӧλ ��1
            E[index][bit / 5] |= (1 << (bit - (bit / 5) * 5));
        }
        index++;
    }
    // cout << E[6][50];
    // print();
//--------------------------------
    // ʹ��C++11��chrono������ʱ
    auto start = chrono::high_resolution_clock::now();
    solve();
    auto end = chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
    cout << diff.count() << "ms" << endl;
//--------------------------------
    // ��֤�����ȷ��
    // print();
    return 0;
}