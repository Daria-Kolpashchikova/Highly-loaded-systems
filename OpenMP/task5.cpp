#include "omp.h"
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

int *oddEvenSort(int n, int *a) {
    int t;
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < n; j += 2) {
                if (a[j] < a[j - 1]) {
                    t = a[j];
                    a[j] = a[j - 1];
                    a[j - 1] = t;
                }
            }
        }
        else {
            for (int j = 1; j < n; j += 2) {
                if (a[j] < a[j - 1]) {
                    t = a[j];
                    a[j] = a[j - 1];
                    a[j - 1] = t;
                }
            }

        }
    }
    return a;
}

int* oddEvenSortParallel(int n, int* a, int threads) {
    int t;
omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < n; j += 2) {
                if (a[j] < a[j - 1]) {
                    t = a[j];
                    a[j] = a[j - 1];
                    a[j - 1] = t;
                }
            }
        }
        else {
            for (int j = 1; j < n; j += 2) {
                if (a[j] < a[j - 1]) {
                    t = a[j];
                    a[j] = a[j - 1];
                    a[j - 1] = t;
                }
            }

        }
    }
    return a;
}

int* generate_vector(int n)
{
    int* arr = new int[n];
    srand(time(NULL));
    for (int i = 0; i < n; ++i)
    {
        arr[i] = rand() % 10;
    }
    return arr;
}

void run_test(int numberTests, int n, int threads) {
    int* arr = new int[n];
    double real_time_s = 0, real_time_p = 0;
    std::cout << "number of iter: " << numberTests << " number of threads: " << threads << " size of arrays: " << n << std::endl;
    for (int i = 0; i < numberTests; i++) {
        arr = generate_vector(n);
        auto t_start_s = std::chrono::high_resolution_clock::now();
        oddEvenSort(n, arr);
        auto t_end_s = std::chrono::high_resolution_clock::now();
        real_time_s += std::chrono::duration<double, std::milli>(t_end_s - t_start_s).count();

        auto t_start_p = std::chrono::high_resolution_clock::now();
        oddEvenSortParallel(n, arr, threads);
        auto t_end_p = std::chrono::high_resolution_clock::now();
        real_time_p += std::chrono::duration<double, std::milli>(t_end_p - t_start_p).count();
    }

    std::cout << "sequent program time: " << real_time_s / numberTests << " ms" << std::endl;
    std::cout << "parallel program time: " << real_time_p  / numberTests << " ms" << std::endl << std::endl;
}

int main() {
    run_test(50, 100, 2);
    run_test(50, 100, 4);
    run_test(50, 100, 8);

    run_test(100, 200, 2);
    run_test(100, 200, 4);
    run_test(100, 200, 8);

    run_test(200, 1000, 2);
    run_test(200, 1000, 4);
    run_test(200, 1000, 8);
}
