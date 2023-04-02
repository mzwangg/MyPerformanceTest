#pragma once
#include "config.h"

double cpuIntTest(int n)
{
    const int NUM_ITERATIONS = 1000000000; // ��������
    int a = 0, b = 1, c;

    auto start_time = chrono::high_resolution_clock::now(); // ��¼��ʼʱ��

    for (int i = 0; i < n; ++i) {
        c = a + b; // �����ӷ�
        a = b;
        b = c;
    }

    auto end_time = chrono::high_resolution_clock::now(); // ��¼����ʱ��
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time); // ��������ʱ��
    return duration.count() / CLOCKS_PER_SECOND;
}

double cpuDoubleTest(int n)
{
    double sum = 0.0, step = 1.0 / n, x;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++)
    {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double pi = sum * step;
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    return duration.count() / CLOCKS_PER_SECOND;
}

extern double cpuTest()
{
    double intTime = 0;
    double doubleTime = 0;

    //�������岿��
    for (int size = 1 << 12; size <= CPU_TEST_SIZE; size <<= 3)
    {
        double localIntTime = cpuIntTest(size);
        double localDoubleTime = cpuDoubleTest(size);

        intTime += localIntTime * (CPU_TEST_SIZE / size);
        doubleTime += localDoubleTime * (CPU_TEST_SIZE / size);
    }

    double intScore = CPU_INT_BASE_TIME / intTime;
    double doubleScore = CPU_DOUBLE_BASE_TIME / doubleTime;

    double cpuScore = sqrt(intScore * doubleScore);
    return cpuScore;
}