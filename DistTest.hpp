#pragma once
#include "config.h"

//����һ���������������������
default_random_engine generator;

//���������д���ܵĺ���
double randomTest(int block_count, char* buffer)
{
    uniform_int_distribution<int> distribution(0, block_count - 1);

    //ִ�������д����
    ifstream read_file("tempfile", ios::in | ios::binary);
    ofstream write_file("tempfile", ios::out | ios::binary);

    chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();

    //������
    for (int i = 0; i < block_count; i++) {
        int offset = distribution(generator) * BLOCK_SIZE;
        read_file.seekg(offset, ios::beg);
        read_file.read(buffer, BLOCK_SIZE);
        write_file.seekp(offset, ios::beg);
        write_file.write(buffer, BLOCK_SIZE);
    }

    chrono::time_point<chrono::high_resolution_clock> stop = chrono::high_resolution_clock::now();
    double time = chrono::duration_cast<chrono::nanoseconds>(stop - start).count() / CLOCKS_PER_SECOND;

    //double throughput = block_count * BLOCK_SIZE / time / (1024 * 1024);
    //double iops = block_count * 2 / time;

    //����
    read_file.close();
    write_file.close();
    return time;
}

//����˳���д���ܵĺ���
double sequentialTest(int block_count, char* buffer)
{
    //ִ�������д����
    ifstream read_file("tempfile", ios::in | ios::binary);
    ofstream write_file("tempfile", ios::out | ios::binary);

    chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();

    //������
    for (int i = 0; i < block_count; i++) {
        read_file.seekg(i, ios::beg);
        read_file.read(buffer, BLOCK_SIZE);
        write_file.seekp(i, ios::beg);
        write_file.write(buffer, BLOCK_SIZE);
    }

    chrono::time_point<chrono::high_resolution_clock> stop = chrono::high_resolution_clock::now();
    double time = chrono::duration_cast<chrono::nanoseconds>(stop - start).count() / CLOCKS_PER_SECOND;

    //double throughput = block_count * BLOCK_SIZE / time / (1024 * 1024);
    //double iops = block_count * 2 / time;

    //����
    read_file.close();
    write_file.close();
    return time;
}

extern double distTest()
{
    //������ʱ�ļ�
    char* buffer = new char[BLOCK_SIZE];
    ofstream file("tempfile", ios::out | ios::binary | ios::trunc);
    int blockCount = DIST_TEST_SIZE / BLOCK_SIZE / sizeof(char);
    for (int i = 0; i < blockCount; i++) {
        file.write(buffer, BLOCK_SIZE);
    }
    file.close();

    double randomTime = 0;
    double sequentialTime = 0;
    for (int size = 1 << 13; size <= MEMORY_TEST_SIZE; size <<= 4)
    {
        int block_count = size / BLOCK_SIZE / sizeof(char);

        double localRandomTime = randomTest(block_count, buffer);
        double localSequentialTime = sequentialTest(block_count, buffer);

        randomTime += localRandomTime * (DIST_TEST_SIZE / size);
        sequentialTime += localSequentialTime * (DIST_TEST_SIZE / size);
    }

    remove("tempfile");
    delete[] buffer;
    double randomScore = DIST_RANDOM_BASE_TIME/ randomTime;
    double sequentiallScore = DIST_SEQUENTIAL_BASE_TIME/ sequentialTime;

    return sqrt(randomScore * sequentiallScore);
}