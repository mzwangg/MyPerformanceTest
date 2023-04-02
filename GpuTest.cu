#pragma once
#include <string.h>
#include <opencv2/opencv.hpp>
#include "config.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define CHECK(call)\
{\
  const cudaError_t error=call;\
  if(error!=cudaSuccess)\
  {\
      printf("ERROR: %s:%d,",__FILE__,__LINE__);\
      printf("code:%d,reason:%s\n",error,cudaGetErrorString(error));\
      exit(1);\
  }\
}

//random_device rd;
//default_random_engine eng(rd());
//uniform_real_distribution<double> distr(0, 100);

//template<typename T>
//void initialData(T* ip, int size)
//{
//    for (int i = 0; i < size; i++)
//    {
//        ip[i] = (T)distr(eng);
//    }
//}

template<typename T>
void printMatrix(T* C, const int nx, const int ny)
{
    T* ic = C;
    printf("Matrix<%d,%d>:", ny, nx);
    for (int i = 0; i < ny; i++)
    {
        for (int j = 0; j < nx; j++)
        {
            printf("%6f ", ic[j]);
        }
        ic += nx;
        printf("\n");
    }
}

void initDevice(int devNum)
{
    int dev = devNum;
    cudaDeviceProp deviceProp;
    CHECK(cudaGetDeviceProperties(&deviceProp, dev));
    if (GPU_CHECK)printf("Using device %d: %s\n", dev, deviceProp.name);
    CHECK(cudaSetDevice(dev));
}

template<typename T>
void checkResult(T* hostRef, T* gpuRef, const int N)
{
    double eps = 1e-8;
    for (int i = 0; i < N; i++)
    {
        if (abs(hostRef[i] - gpuRef[i]) > eps)
        {
            printf("Results don\'t match!\n");
            printf("%f(hostRef[%d] )!= %f(gpuRef[%d])\n", hostRef[i], i, gpuRef[i], i);
            return;
        }
    }
    printf("Check result success!\n");
}

template<typename T>
void matrixMulCPU(T* A, T* B, T* C, int n)
{
    memset(C, 0, n * n * sizeof(T));
    for (int i = 0; i < n; i++)
    {
        int ik = i * n;
        for (int k = 0; k < n; k++)
        {
            int ij = i * n;
            int kj = k * n;
            for (int j = 0; j < n; j++)
            {
                C[ij] += A[ik] * B[kj];
                ij++;
                kj++;
            }
            ik++;
        }
    }
}

template<typename T>
__global__ void matrixMulGPU(T* A, T* B, T* C, int n)
{
    int nRow = blockIdx.x;
    int nCol = threadIdx.x;
    T fCVal = 0;

    for (int i = 0; i < n; i++)
    {
        fCVal += A[nRow * n + i] * B[i * n + nCol];
    }

    C[nRow * n + nCol] = fCVal;
}

template<typename T>
double gpuCalculateTest(int n, T* A_host, T* B_host, T* C_from_gpu, T* C_host = NULL)
{
    initDevice(0);
    int nxy = n * n;
    int nBytes = nxy * sizeof(T);

    //cudaMalloc
    T* A_dev = NULL;
    T* B_dev = NULL;
    T* C_dev = NULL;
    CHECK(cudaMalloc((void**)&A_dev, nBytes));
    CHECK(cudaMalloc((void**)&B_dev, nBytes));
    CHECK(cudaMalloc((void**)&C_dev, nBytes));
    CHECK(cudaMemcpy(A_dev, A_host, nBytes, cudaMemcpyHostToDevice));
    CHECK(cudaMemcpy(B_dev, B_host, nBytes, cudaMemcpyHostToDevice));

    // 1d block and 1d grid
    dim3 block(n);
    dim3 grid((nxy - 1) / block.x + 1);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    matrixMulGPU<T> << <grid, block >> > (A_dev, B_dev, C_dev, n);
    CHECK(cudaDeviceSynchronize());

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float time;
    cudaEventElapsedTime(&time, start, stop);

    if (GPU_CHECK)
    {
        printf("GPU Execution configuration<<<(%d,%d),(%d,%d)>>> Time elapsed %f sec\n",
            grid.x, grid.y, block.x, block.y, time);
        matrixMulCPU<T>(A_host, B_host, C_host, n);
        CHECK(cudaMemcpy(C_from_gpu, C_dev, nBytes, cudaMemcpyDeviceToHost));
        checkResult(C_host, C_from_gpu, nxy);
    }

    cudaFree(A_dev);
    cudaFree(B_dev);
    cudaFree(C_dev);
    cudaDeviceReset();
    return time / 1000.0;
}

template<typename T>
double gpuCalTypeTest()
{
    int nBytes = GPU_N * GPU_N * sizeof(T);
    T* A = (T*)malloc(nBytes);
    T* B = (T*)malloc(nBytes);
    T* C_from_gpu = (T*)malloc(nBytes);

    //initialData<T>(A, GPU_N * GPU_N);
    //initialData<T>(B, GPU_N * GPU_N);

    double time = 0;
    if (GPU_CHECK)
    {
        T* C = (T*)malloc(nBytes);
        time += gpuCalculateTest<T>(GPU_N, A, B, C_from_gpu, C);
        free(C);
    }
    else
    {
        for (int n = 1 << 6; n <= GPU_N; n <<= 1)
        {
            time += gpuCalculateTest<T>(n, A, B, C_from_gpu) * pow(GPU_N / n, 3);
        }
    }

    free(A);
    free(B);
    free(C_from_gpu);
    return time;
}

__global__ void gaussianBlur(unsigned char* src, unsigned char* dst, int width, int height, float sigma)
{
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    if (x < width && y < height)
    {
        float sum = 0.0f;
        float pixel = 0.0f;
        float weight = 0.0f;

        for (int j = -2; j <= 2; j++)
        {
            for (int i = -2; i <= 2; i++)
            {
                int xIndex = x + i;
                int yIndex = y + j;

                if (xIndex >= 0 && yIndex >= 0 && xIndex < width && yIndex < height)
                {
                    pixel = src[yIndex * width + xIndex];
                    weight = exp(-(i * i + j * j) / (2 * sigma * sigma)) / (3.1415926f * 2 * sigma * sigma);
                    sum += weight * pixel;
                }
            }
        }
        dst[y * width + x] = (unsigned char)sum;
    }
}

double gpuGraphtest()
{
    float time = 0;
    for (int step = 0; step < 10; step++)
    {
        for (int i = 1; i <= 5; i++)
        {
            string inputPath = "InputPic/" + to_string(i) + ".png";
            cv::Mat image = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);

            int width = image.cols;
            int height = image.rows;
            int size = width * height * sizeof(unsigned char);

            unsigned char* h_inputImage = (unsigned char*)image.data;
            unsigned char* h_outputImage = (unsigned char*)malloc(size);
            unsigned char* d_inputImage;
            unsigned char* d_outputImage;

            cudaMalloc((void**)&d_inputImage, size);
            cudaMalloc((void**)&d_outputImage, size);
            cudaMemcpy(d_inputImage, h_inputImage, size, cudaMemcpyHostToDevice);

            dim3 threadsPerBlock(16, 16);
            dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x, (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

            cudaEvent_t start, stop;
            cudaEventCreate(&start);
            cudaEventCreate(&stop);
            cudaEventRecord(start, 0);

            gaussianBlur << <numBlocks, threadsPerBlock >> > (d_inputImage, d_outputImage, width, height, SIGMA);

            cudaEventRecord(stop, 0);
            cudaEventSynchronize(stop);
            float localTime = 0;
            cudaEventElapsedTime(&localTime, start, stop);

            cudaFree(d_inputImage);
            cudaFree(d_outputImage);
            free(h_outputImage);
            time += localTime;
        }
    }
    return (double)time / 1000.0;
}

extern "C" double gpuTest()
{
    //测量主体部分
    double intTime = gpuCalTypeTest<int>();
    double doubleTime = gpuCalTypeTest<double>();
    double graphTime = gpuGraphtest();

    double intScore = GPU_INT_BASE_TIME / intTime;
    double doubleScore = GPU_DOUBLE_BASE_TIME / doubleTime;
    double graphScore = GPU_GRAPH_BASE_TIME / graphTime;

    double gpuScore = pow(intScore * doubleScore * graphScore, 1.0 / 3);
    return gpuScore;
}