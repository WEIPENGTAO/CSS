/*
这是一个矩阵乘法的并行实现
测试了串行和并行两种实现方式的运行时间
并进行了两项测试：
1. 线程数量一定（和核数一样）矩阵规模不断变化，甚至超过 cache 大小，接近内存大小
2. 计算规模一定（不要太小，否则串行比并行还快），线程数量不断变化，从 1 到 核数 * 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void serial(int m, float **&a, float **&b, float **&c); // 串行计算函数
void parallel(int m, float **&a, float **&b, float **&c); // 并行计算函数

// 设置矩阵大小
void parallel_size(int m);

// 1. 线程数量一定（和核数一样）矩阵规模不断变化，甚至超过 cache 大小，接近内存大小
void test1();

// 2. 计算规模一定（不要太小，否则串行比并行还快），线程数量不断变化，从 1 到 核数 * 2
void test2();

int main()
{
    freopen("output.txt", "w", stdout); // 将输出重定向到文件中
    // test1();
    test2();

    fclose(stdout); // 关闭文件
    return 0;
}

void serial(int m, float **&a, float **&b, float **&c)
{
    double start, end;
    int i, j, k;

    start = omp_get_wtime(); // 记录串行计算开始时间
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            for (k = 0; k < m; k++)
            {
                c[i][j] = c[i][j] + a[i][k] * b[k][j]; // 计算C矩阵的每个元素
            }
        }
    }
    end = omp_get_wtime(); // 记录串行计算结束时间
    // 打印矩阵大小、矩阵占用空间（以 GB 为单位）以及串行计算时间
    fprintf(stderr, "m = %d, matrix size: %0.6lf GB, serial matrix multiply time: %0.6lf\n",
            m, (double)(m * m * sizeof(float) * 3) / (1024 * 1024 * 1024), end - start);
    printf("m = %d, matrix size: %0.6lf GB, serial matrix multiply time: %0.6lf\n",
           m, (double)(m * m * sizeof(float) * 3) / (1024 * 1024 * 1024), end - start);
}

// 进行矩阵乘法的并行实现
void parallel(int m, float **&a, float **&b, float **&c)
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

// 使用OpenMP并行化矩阵乘法运算
// parallel是并行执行的代码块，shared是共享变量，即所有线程共享变量，private是私有变量，即每个线程都有自己的变量副本
#pragma omp parallel shared(a, b, c) private(j, k)
    {
// 为了减小竞争，使用for循环并行化外层循环
// schedule(dynamic)表示动态调度，即每个线程从循环队列中获取一个任务，执行完后再获取一个任务，直到循环结束
// 由于矩阵乘法的计算量较大，因此使用动态调度可以减小线程之间的竞争
#pragma omp for schedule(dynamic)
        for (i = 0; i < m; i++)
        {
            // 内层循环与串行版本相同
            for (j = 0; j < m; j++)
            {
                for (k = 0; k < m; k++)
                {
                    c[i][j] = c[i][j] + a[i][k] * b[k][j];
                }
            }
        }
    }

    // 获取结束时间
    end = omp_get_wtime();

    fprintf(stderr, "m = %d, matrix size: %0.6lf GB, parallel matrix multiply time: %0.6lf\n",
            m, (double)(m * m * sizeof(float) * 3) / (1024 * 1024 * 1024), end - start);
    printf("m = %d, matrix size: %0.6lf GB, parallel matrix multiply time: %0.6lf\n",
           m, (double)(m * m * sizeof(float) * 3) / (1024 * 1024 * 1024), end - start);
}

void parallel_size(int m)
{
    float **a = new float *[m];
    float **b = new float *[m];
    float **c = new float *[m];
    for (int i = 0; i < m; i++)
    {
        a[i] = new float[m];
        b[i] = new float[m];
        c[i] = new float[m];
    }

    // srand((unsigned)time(NULL)); // 生成随机数种子
    // 保证每次生成的随机数相同
    srand(0);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = (float)rand() / (RAND_MAX);
            b[i][j] = (float)rand() / (RAND_MAX);
            c[i][j] = 0;
        }
    }

    serial(m, a, b, c);
    parallel(m, a, b, c);

    for (int i = 0; i < m; i++)
    {
        delete[] a[i];
        delete[] b[i];
        delete[] c[i];
    }
    delete[] a;
    delete[] b;
    delete[] c;
}

void test1()
{
    double start, end;

    int pnum = omp_get_num_procs();     // 获取处理器核心数
    printf("Thread_pnum = %d\n", pnum); // 打印处理器核心数
    omp_set_num_threads(pnum);          // 设置线程数为处理器核心数

    // parallel_size(256);
    // parallel_size(1024);
    // parallel_size(4096);
    parallel_size(16384);
    parallel_size(65536);
}

void test2()
{
    double start, end;

    int m = 2048;
    float **a = new float *[m];
    float **b = new float *[m];
    float **c = new float *[m];
    for (int i = 0; i < m; i++)
    {
        a[i] = new float[m];
        b[i] = new float[m];
        c[i] = new float[m];
    }

    // srand((unsigned)time(NULL)); // 生成随机数种子
    // 保证每次生成的随机数相同
    srand(0);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = (float)rand() / (RAND_MAX);
            b[i][j] = (float)rand() / (RAND_MAX);
            c[i][j] = 0;
        }
    }

    int pnum = omp_get_num_procs();
    printf("Thread_pnum = %d\n", pnum);

    // 并行线程数从 1 到处理器核心数的 2 倍
    for (int i = 1; i <= pnum * 2; i++)
    {
        printf("Thread_num = %d\n", i);
        omp_set_num_threads(i);
        parallel(m, a, b, c);
    }

    for (int i = 0; i < m; i++)
    {
        delete[] a[i];
        delete[] b[i];
        delete[] c[i];
    }
    delete[] a;
    delete[] b;
    delete[] c;
}