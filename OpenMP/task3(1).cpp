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


 int *mult_mat_arr(int** mat, int* vec, int n) {
     int* result_vector = new int[n];
     for (int i = 0; i < n; i++)
     {
         result_vector[i] = 0;
         for (int j = 0; j < n; j++)
         {
             result_vector[i] += mat[i][j] * vec[j];
         }
     }

     return result_vector;
}

 int* mult_mat_arr_p_str(int** mat, int* vec, int n, int threadsCount) {
  int* result_vector = new int[n];
#pragma omp parallel num_threads(threadsCount) for
     for (int i = 0; i < n; i++)
     {
         result_vector[i] = 0;
         for (int j = 0; j < n; j++)
         {
             result_vector[i] += mat[i][j] * vec[j];
         }
     }

     return result_vector;
 }

int* mult_mat_arr_p_col(int** mat, int* vec, int n, int threadsCount) {
     int* result_vector = new int[n];
#pragma omp parallel num_threads(threadsCount)
     {
         int* result_thread = new int[n];

#pragma omp for
         for (int j = 0; j < n; j++) {
             result_vector[j] = 0;
             result_thread[j] = 0;
             for (int i = 0; i < n; i++) {
                 result_thread[i] += mat[i][j] * vec[j];
             }
         }
#pragma omp critical
         {
             for (int i = 0; i < n; i++) {
                 result_vector[i] += result_thread[i];
             }
         }
     }

     return result_vector;
 }

int *mult_block_arr(int** mat, int* vec, int n, int thread_num) {
    int row_step, col_step;
    int left, right, top, bottom;

    int* result_vector = new int[n];
    row_step = n / 2;
    col_step = n / omp_get_max_threads() * 2;
    left = row_step * (thread_num % 2);
    right = left + row_step;
    top = col_step * (thread_num / 2);
    bottom = top + col_step;
    

    int temp = 0;
    for (int i = 0; i < n; i++) {
        result_vector[i] = 0;
    }

    for (int i = top; i < bottom; i++) {
        temp = 0;
        for (int j = left; j < right; j++) {
            temp += mat[i][j] * vec[j];
        }
#pragma omp atomic
        result_vector[i] += temp;
    }

    return result_vector;
}


int *mult_mat_arr_block(int** mat, int* vec, int n, int threadsCount) {
    std::vector<int> res(n, 0);
    int* result_vector = new int[n];
#pragma omp parallel num_threads(threadsCount)
    {
        result_vector = mult_block_arr(mat, vec, n, omp_get_thread_num());
    }

    return result_vector;
}

void run_test(int n, int m, int threadsCount) {
    std::cout << "number of threads: " << threadsCount << std::endl;
    double real_time = 0;

        int** matrix = generate_matrix(n);
        int* vec = generate_vector(n);
        int* res = mult_mat_arr(matrix, vec, n);

        auto t_start = std::chrono::high_resolution_clock::now();

        mult_mat_arr(matrix, vec, n);

        auto t_end = std::chrono::high_resolution_clock::now();
       
        real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();

        std::cout << "sequential program time: " << real_time << " ms" << std::endl;

        t_start = std::chrono::high_resolution_clock::now();

        mult_mat_arr_p_str(matrix, vec, n, threadsCount);

        t_end = std::chrono::high_resolution_clock::now();
        real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
        std::cout << "parallel program with line splitting: " << real_time << " ms" << std::endl;

        t_start = std::chrono::high_resolution_clock::now();

        mult_mat_arr_p_col(matrix, vec, n, threadsCount);

        t_end = std::chrono::high_resolution_clock::now();
        real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
       std::cout << "parallel program with column splitting: " << real_time << " ms" << std::endl;

        t_start = std::chrono::high_resolution_clock::now();
   
        mult_mat_arr_block(matrix, vec, n, threadsCount);

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
