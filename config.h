#pragma once
#include <iostream>
#include <chrono>
#include <random>
#include <math.h>
#include <fstream>
using namespace std;

const double CLOCKS_PER_SECOND = 1e9;

//CpuTest
const int CPU_TEST_SIZE = 1 << 24;
const double CPU_INT_BASE_TIME = 0.101978;
const double CPU_DOUBLE_BASE_TIME = 0.282611;

//GpuTest
const bool GPU_CHECK = false;
const int GPU_N = 512;
const float SIGMA = 1;
const double GPU_INT_BASE_TIME = 0.0700457;
const double GPU_DOUBLE_BASE_TIME = 0.0914726;
const double GPU_GRAPH_BASE_TIME = 8.1632e-05;

//MemoryTest
const int MEMORY_TEST_SIZE = 1024 * 1024 * 256; // 256MB
const int CACHE_LINE_SIZE = 64;
const double MEMORY_RANDOM_BASE_TIME = 22.482;
const double MEMORY_SEQUENTIAL_BASE_TIME = 0.716822;
const double MEMORY_NO_CACHE_BASE_TIME = 1.9927;

//DistTest
const int DIST_TEST_SIZE = 1024 * 1024 * 32; // 32MB
const int BLOCK_SIZE = 1024 * 4;
const double DIST_RANDOM_BASE_TIME = 1.23941;
const double DIST_SEQUENTIAL_BASE_TIME = 0.958934;