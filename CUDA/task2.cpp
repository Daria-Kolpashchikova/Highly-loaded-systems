#include <cuda_runtime.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <chrono>
using namespace std;

double cpuSecond() {
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return ((double)tp.tv_sec + (double)tp.tv_usec*1.e-6);
}

__global__ void sumMatrixOnGPU2D(float *MatA, float *MatB, float *MatC,
int nx, int ny) {
  unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
  unsigned int iy = threadIdx.y + blockIdx.y * blockDim.y;
  unsigned int idx = iy*nx + ix;
  if (ix < nx && iy < ny){
   MatC[idx] = MatA[idx] + MatB[idx];
  }
}

__global__ void sumMatrixOnGPUMix(float *MatA, float *MatB, float *MatC,
int nx, int ny) {
 unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
 unsigned int iy = blockIdx.y;
 unsigned int idx = iy*nx + ix;
 if (ix < nx && iy < ny){
  MatC[idx] = MatA[idx] + MatB[idx];
 }
}

__global__ void sumMatrixOnGPU1D(float *MatA, float *MatB, float *MatC,
int nx, int ny) {
 unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
 if (ix < nx ) {
  for (int iy=0; iy<ny; iy++) {
   int idx = iy*nx + ix;
   MatC[idx] = MatA[idx] + MatB[idx];
  }
 }
}

void generateArr(float* arr, int size)
{
    srand(time(NULL));
    for (int i=0; i<size; ++i){
        arr[i] = rand()% 10;
    }
}

void func1D(float* Ap, float* Bp, float *A, float *B, float *C, int size, int nx, int ny, int blockParam1, int blockParam2){
    double time;
    float* Cp;
    cudaMalloc((void**)&Cp, size);

    cudaDeviceSynchronize();
    time = cpuSecond();
    dim3 block(blockParam1,blockParam2);
    dim3 grid((nx+block.x-1)/block.x,1);
    sumMatrixOnGPU1D<<< grid, block >>>(Ap, Bp, Cp, nx, ny);
    cudaDeviceSynchronize();
    time = cpuSecond() - time;
    printf("1D: <<<(%d, %d)>>> time: %f ms\n", grid,
          block, time);

    cudaMemcpy(C, Cp, size, cudaMemcpyDeviceToHost);  
    cudaFree(Cp);   
}

void func2D(float* Ap, float* Bp, float *A, float *B, float *C, int size, int nx, int ny, int blockParam1, int blockParam2){
    double time;
    float* Cp; ///?
    cudaMalloc((void**)&Cp, size);

    int dimx = blockParam1;
    int dimy = blockParam2;
    dim3 block(dimx, dimy);
    dim3 grid((nx + block.x - 1) / block.x, (ny + block.y - 1) / block.y);

    cudaDeviceSynchronize();
    time = cpuSecond();
    sumMatrixOnGPU2D<<<grid, block>>>(Ap, Bp, Cp, nx, ny);
    cudaDeviceSynchronize();
    time = cpuSecond() - time;
    printf("2D: <<<(%d, %d), (%d, %d)>>> time: %f ms\n", grid.x,
           grid.y,
          block.x, block.y, time);

    cudaMemcpy(C, Cp, size, cudaMemcpyDeviceToHost);
    cudaFree(Cp);
}

void funcMix(float* Ap, float* Bp, float *A, float *B, float *C, int size, int nx, int ny, int blockParam){
   double time;
   float* Cp;
   cudaMalloc((void**)&Cp, size);
    
   dim3 block(blockParam);
   dim3 grid((nx + block.x - 1) / block.x,ny);

   cudaDeviceSynchronize();
   time = cpuSecond();
   sumMatrixOnGPUMix <<<grid, block>>> (Ap, Bp, Cp, nx, ny);
   cudaDeviceSynchronize();
   time = cpuSecond() - time;
   printf("Mix: <<<(%d, %d), (%d, %d)>>> time: %f ms\n", grid.x,
           grid.y,
           block.x, block.y, time);
   cudaMemcpy(C, Cp, size, cudaMemcpyDeviceToHost);
   cudaFree(Cp);
  
}


void CUDAunit(float* Ap, float* Bp, float *A, float *B, int size){

  cudaMalloc((void**)&Ap, size);
  cudaMalloc((void**)&Bp, size);

  cudaMemcpy(Ap, A, size, cudaMemcpyHostToDevice);
  cudaMemcpy(Bp, B, size, cudaMemcpyHostToDevice);
}

void cudaEnd(float* Ap, float* Bp, float *A, float *B, float *C){
  cudaFree(Ap);
  cudaFree(Bp);

  free(A);
  free(B);
  free(C);
}

int main(void) {

    unsigned int nx = 1<<11; 
    unsigned int ny = 1<<11; 
    int N = nx*ny;
    size_t size = N * sizeof(int);
    
    float* A = (float*)malloc(size);
    float* B = (float*)malloc(size);
    float* C = (float*)malloc(size);
    float* Ap;
    float* Bp;
    generateArr(A, N);
    generateArr(B, N);
    CUDAunit(Ap, Bp, A, B, size);
    func1D(Ap, Bp, A, B, C, size, nx, ny, 32, 1);
    func2D(Ap, Bp, A, B, C, size, nx, ny, 32, 32);
    funcMix(Ap, Bp, A, B, C, size, nx, ny, 32);
    cout << "my config:" <<  endl;
    //my config
    func1D(Ap, Bp, A, B, C, size, nx, ny, 256, 1);
    func2D(Ap, Bp, A, B, C, size, nx, ny, 64, 32);
    funcMix(Ap, Bp, A, B, C, size, nx, ny, 256);
    
    cudaEnd(Ap, Bp, A, B, C);
    
    return 0;
}
