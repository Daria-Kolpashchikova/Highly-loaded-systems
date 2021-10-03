#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <omp.h>


int** generate_matrix(int n)
{
    int** arr = new int* [n];
    srand(time(NULL));
    for (int i = 0; i < n; ++i)
    {
        arr[i] = new int[n];
        for (int j = 0; j < n; ++j)
        {
            arr[i][j] = rand() % 10;
        }
    }
    return arr;
}


int **mult_mat_mat(int** mat1, int** mat2, int n) {
    int** res = new int* [n];
    int sum;
    for (int i = 0; i < n; ++i)
    {
        res[i] = new int[n];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat1[i][k] * mat2[k][j];
            }
            res[i][j] = sum;
        }
    }

    return res;
}

int** mult_mat_mat_p(int** mat1, int** mat2, int n, int countThreads) { 
    int** res = new int* [n];

    int sum;
    for (int i = 0; i < n; ++i)
    {
        res[i] = new int[n];
    }

#pragma omp parallel num_threads(countThreads) for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat1[i][k] * mat2[k][j];
            }
            res[i][j] = sum;
        }
    }

    return res;
}

int** mult_block_arr(int** mat1, int** mat2, int n, int thread_num) {
    int row_step, col_step;
    int left, right, top, bottom;


        row_step = n / 2;
        col_step = n / omp_get_max_threads() * 2;
        left = row_step * (thread_num % 2);
        right = left + row_step;
        top = col_step * (thread_num / 2);
        bottom = top + col_step;
    
        int** res = new int* [n];

        int sum;
        for (int i = 0; i < n; ++i)
        {
            res[i] = new int[n];
        }

    for (int i = top; i < bottom; i++) {
        for (int j = left; j < right; j++) {
            sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat1[i][k] * mat2[k][j];
            }
            res[i][j] = sum;
        }
    }
    return res;
}
int** mult_mat_mat_block(int** mat1, int** mat2, int n, int countThreads) {
    int** res = new int* [n];

    for (int i = 0; i < n; ++i)
    {
        res[i] = new int[n];
    }

#pragma omp parallel num_threads(countThreads)
    {
       res = mult_block_arr(mat1, mat2, n, omp_get_thread_num());
    }

    return res;
}

void run_test(int n, int m, int threadsCount) {
    std::cout << "number of threads: " << threadsCount << std::endl;
    double real_time = 0;

    int** A = generate_matrix(n);
    int** B = generate_matrix(n);
    int** res = new int* [n];

    for (int i = 0; i < n; ++i)
    {
        res[i] = new int[n];
    }

    auto t_start = std::chrono::high_resolution_clock::now();
    mult_mat_mat(A, B, n);
    auto t_end = std::chrono::high_resolution_clock::now();

    real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "sequential program time: " << real_time << " ms" << std::endl;

    t_start = std::chrono::high_resolution_clock::now();

    mult_mat_mat_p(A, B, n, threadsCount);

    t_end = std::chrono::high_resolution_clock::now();
    real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "parallel program with tape splitting: " << real_time << " ms" << std::endl;

    t_start = std::chrono::high_resolution_clock::now();

    mult_mat_mat_block(A, B, n, threadsCount);

    t_end = std::chrono::high_resolution_clock::now();
    real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "parallel program with block splitting: " << real_time << " ms" << std::endl;
}

int main() {
    run_test(200, 200, 2);
    run_test(400, 400, 4);
    run_test(1000, 1000, 8);

    return 0;
}
