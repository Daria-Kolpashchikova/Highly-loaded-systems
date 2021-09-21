#include <iostream>

#include <random>

#include <iomanip>

#include <ctime>

#include <chrono>

#include <mpi.h>

using namespace std;

bool check(int * arr, int size) {
  for (int i = 0; i < size - 1; i++)
    if (arr[i] > arr[i + 1])
      return false;

  return true;
}

int * generateArr(int arrSize) {

  int * arr = new int[arrSize];
  srand(time(NULL));

  for (int i = 0; i < arrSize; i++) {
    arr[i] = rand() % 100;
  }

  return arr;
}

void mergeSort(int arr[], int l, int m, int r) {
  int n1 = m - l + 1, n2 = r - m, L[n1], R[n2];

  for (int i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  int i = 0, j = 0, k = l;

  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

void mergeMainFunc(int arr[], int l, int r) {
  if (l >= r) {
    return;
  }
  int m = (l + r - 1) / 2;
  mergeMainFunc(arr, l, m);
  mergeMainFunc(arr, m + 1, r);
  mergeSort(arr, l, m, r);
}

void MPIMergeSort(int * arr, int l, int r, int thrN, int max_threads, int thrOffset) {

  if (l >= r) {
    return;
  }

  int n = l + (r - l) / 2;
  int nextThr = thrN + thrOffset;

  if (nextThr < max_threads) {
    MPI_Request msg_request;
    MPI_Status msg_status;

    MPI_Isend(arr + n + 1, r - n, MPI_INT, nextThr, thrOffset * 2, MPI_COMM_WORLD, & msg_request);
    MPIMergeSort(arr, l, n, thrN, max_threads, thrOffset * 2);
    MPI_Recv(arr + n + 1, r - n, MPI_INT, nextThr, thrOffset * 2, MPI_COMM_WORLD, & msg_status);
    mergeSort(arr, l, n, r);
  } else {
    MPIMergeSort(arr, l, n, thrN, max_threads, thrOffset * 2);
    MPIMergeSort(arr, n + 1, r, thrN, max_threads, thrOffset * 2);
    mergeSort(arr, l, n, r);
  }
}

void testSequent(int arrSize, int arrCount) {
  double time = 0;
  for (int i = 0; i < arrCount; i++) {
    int * arr = generateArr(arrSize);

    auto begin = chrono::high_resolution_clock::now();
    mergeMainFunc(arr, 0, arrSize - 1);
    auto end = chrono::high_resolution_clock::now();

    if (!check(arr, arrSize)) {
      cout << "Failed" << endl;
    }
    delete[] arr;

    time += chrono::duration < double, milli > (end - begin).count();
  }
  cout << "Size of arrays: " << arrSize << ", count of arrays: " << arrCount << ", time: " << time / arrCount << " ms" << endl;
}

void testMPI(int arrSize, int arrCount) {
  int max_threads, thread_num;
  MPI_Comm_size(MPI_COMM_WORLD, & max_threads);
  MPI_Comm_rank(MPI_COMM_WORLD, & thread_num);

  double time = 0;

  for (int i = 0; i < arrCount; i++) {
    int * arr = generateArr(arrSize);
    auto begin = chrono::high_resolution_clock::now();
    MPIMergeSort(arr, 0, arrSize - 1, thread_num, max_threads, 1);
    auto end = chrono::high_resolution_clock::now();

    if (!check(arr, arrSize)) {
      cout << "Array size error" << arrSize << endl;
    }
    delete[] arr;

    time += chrono::duration < double, milli > (end - begin).count();
  }

  cout << "Size of arrays: " << arrSize << ", count of arrays: " << arrCount << ", time: " << time / arrCount << " ms" << endl;
}

void MPIBegin(int argc, char ** argv) {
  int thrMax, thrNum;
  MPI_Status status;
  MPI_Init( & argc, & argv);
  MPI_Comm_size(MPI_COMM_WORLD, & thrMax);
  MPI_Comm_rank(MPI_COMM_WORLD, & thrNum);

  if (thrNum == 0) {
    return;
  } else {
    while (true) {
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, & status);
      int st = status.MPI_TAG, source = status.MPI_SOURCE;
      if (st == 0) {
        int buf;
        MPI_Recv( & buf, 0, MPI_INT, source, st, MPI_COMM_WORLD, & status);
        MPI_Finalize();
        exit(0);
      } else {
        int * arr, arrSize;
        MPI_Get_count( & status, MPI_INT, & arrSize);
        arr = new int[arrSize];
        MPI_Recv(arr, arrSize, MPI_INT, source, st, MPI_COMM_WORLD, & status);
        MPIMergeSort(arr, 0, arrSize - 1, thrNum, thrMax, st);
        MPI_Send(arr, arrSize, MPI_INT, source, st, MPI_COMM_WORLD);
        delete[] arr;
      }
    }
  }
}

void MPIend(int maxThr) {
  for (int i = 1; i < maxThr; i++) {
    MPI_Send(0, 0, MPI_INT, i, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
}

int main(int argc, char ** argv) {
  int maxThr;
  cout << "Sequential tests: " << endl;
  testSequent(1000, 100);
  testSequent(10000, 200);
  testSequent(20000, 400);
  testSequent(30000, 500);
  testSequent(40000, 800);
  testSequent(50000, 1000);

  MPIBegin(argc, argv);
  MPI_Comm_size(MPI_COMM_WORLD, & maxThr);

  cout << "Parallel tests: " << endl;
  testMPI(1000, 100);
  testMPI(10000, 200);
  testMPI(20000, 400);
  testMPI(30000, 500);
  testMPI(40000, 800);
  testMPI(50000, 1000);

  MPIend(maxThr);

  return 0;
}
