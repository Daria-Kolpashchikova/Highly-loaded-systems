#include <omp.h>
#include <iostream>
using namespace std;

int main() {
    int N, M, S = 1;
    double **A, *B, *strings, *colums, *blocks, t;

    cout << "N: ";
    cin >> N;
    cout << "M: ";
    cin >> M;

    A = new double* [N];
    B = new double[M];

    for (int i = 0; i < N; ++i) {
        A[i] = new double[M];
        for (int j = 0; j < M; ++j) {
            A[i][j] = rand() % 10;
        }
        B[i] = rand() % 10;
    }

    strings = new double[M];
    colums = new double[M];
    blocks = new double[M];   

#pragma omp parallel for
    for (int i = 0; i < M; ++i) {
        strings[i] = 0;
        blocks[i] = 0;
        colums[i] = 0;
    }

    do {
        t = omp_get_wtime() * 1000;
#pragma omp parallel for shared(A,vec,vecStr) num_threads(S)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
#pragma omp atomic
                strings[i] += A[i][j] * B[j];
            }
        }
        t = omp_get_wtime() * 1000 - t;
        cout << "strings, t: " << t << endl;

        t = omp_get_wtime() * 1000;
#pragma omp parallel for shared(A,vec,colums) num_threads(S)
        for (int j = 0; j < M; ++j) {
            for (int i = 0; i < M; ++i) {
#pragma omp atomic
                colums[i] += A[i][j] * B[j];
            }
        }
        t = omp_get_wtime() * 1000 - t;
        cout << "colums, t: " << t << endl;

        t = omp_get_wtime() * 1000;
        int p = N / S;
        int q = M / S;
#pragma omp parallel shared(A,vec,blocks) num_threads(S)
        for (int numBl = 0; numBl < S * S; ++numBl) {
            int i = numBl / S;
            int j = numBl % S;
            for (int k = i * p; k < (i + 1) * p; ++k) {
                for (int l = j * q; l < (j + 1) * q; ++l) {
#pragma omp critical
                    blocks[k] = A[k][l] * B[l];
                }
            }
        }
        t = omp_get_wtime() * 1000 - t;
        cout << "blocks, t: " << t << endl;
    } while (S < omp_get_thread_num());
    return 0;
}
