#pragma once
#include"config.h"

// �����д�ڴ�
double randomTest(int* data, int size)
{
    int temp = 0;
    auto start = chrono::high_resolution_clock::now();

    //�����ȡ
    for (int i = 0; i < size; i++)
    {
        int index = rand() % size;
        temp = data[index];
    }

    //���д��
    for (int i = 0; i < size; i++)
    {
        int index = rand() % size;
        data[index] = temp;
    }

    auto stop = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::nanoseconds>(stop - start).count() / CLOCKS_PER_SECOND;
}

// ˳������ڴ棬�ɽ�������
double sequentialTest(int* data, int size)
{
    int temp = 0;
    auto start = chrono::high_resolution_clock::now();

    //˳���ȡ
    for (int i = 0; i < size; i++)
    {
        temp = data[i];
    }

    //˳��д��
    for (int i = 0; i < size; i++)
    {
        temp = data[i];
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    return duration.count() / CLOCKS_PER_SECOND;
}

//˳���д�ڴ棬���ɽ�������
double noCacheTest(int* data, int size)
{
    int offset = 0;
    int temp = 0;
    int stepLen = CACHE_LINE_SIZE / sizeof(int);
    auto start = chrono::high_resolution_clock::now();

    //ѭ���ȡ
    for (int i = 0; i < size; i++)
    {
        offset += stepLen;
        offset = (offset > size ? offset %= size : offset);
        temp = data[offset];
    }

    //˳��д��
    for (int i = 0; i < size; i++)
    {
        offset += stepLen;
        offset = (offset > size ? offset %= size : offset);
        data[offset] = temp;
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    return duration.count() / CLOCKS_PER_SECOND;
}

extern double memoryTest()
{
    int* data = new int[MEMORY_TEST_SIZE / sizeof(int)];
    double randomTime = 0;
    double sequentialTime = 0;
    double noCacheTime = 0;

    for (int size = 1 << 6; size <= MEMORY_TEST_SIZE / sizeof(int); size <<= 5)
    {
        double localRandomTime = randomTest(data, size);
        double localSequentialTime = sequentialTest(data, size);
        double localNoCacheTime = noCacheTest(data, size);

        randomTime += localRandomTime * (MEMORY_TEST_SIZE / sizeof(int) / size);
        sequentialTime += localSequentialTime * (MEMORY_TEST_SIZE / sizeof(int) / size);
        noCacheTime += localNoCacheTime * (MEMORY_TEST_SIZE / sizeof(int) / size);
    }

    double randomScore = MEMORY_RANDOM_BASE_TIME/ randomTime;
    double sequentiallScore = MEMORY_SEQUENTIAL_BASE_TIME/ sequentialTime;
    double noCacheScore = MEMORY_NO_CACHE_BASE_TIME/ noCacheTime;

    return pow(randomScore * sequentiallScore * noCacheScore, 1.0 / 3);
}