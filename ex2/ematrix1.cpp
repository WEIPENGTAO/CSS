/*
这是一个矩阵乘法的并行实现
测试了串行和并行两种实现方式的运行时间
并且要调节 for 编译制导中 schedule 的参数，使得执行时间最短
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define m 2000

float a[m][m]; // 矩阵A
float b[m][m]; // 矩阵B
float c[m][m]; // 矩阵C

void serial();           // 串行计算函数
void parallel_dynamic(); // dynamic 调度并行计算函数
void parallel_static();  // static 调度并行计算函数
void parallel_guided();  // guided 调度并行计算函数
void parallel_runtime(); // runtime 调度并行计算函数
void parallel_auto();    // auto 调度并行计算函数

int main()
{
    // freopen("ematrix1.txt", "w", stdout); // 将输出重定向到文件中

    double start, end;

    int i, j, k;
    // srand((unsigned)time(NULL)); // 生成随机数种子
    // 保证每次生成的随机数相同
    srand(0);
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            a[i][j] = (float)rand() / (RAND_MAX); // 生成A矩阵中的随机数
            b[i][j] = (float)rand() / (RAND_MAX); // 生成B矩阵中的随机数
            c[i][j] = 0;                          // 将C矩阵中的每个元素初始化为0
        }
    }

    int pnum = omp_get_num_procs();     // 获取处理器核心数
    omp_set_num_threads(pnum);          // 设置并行线程数为 4
    printf("Thread_pnum = %d\n", pnum); // 打印处理器核心数

    serial();           // 调用串行计算函数
    parallel_dynamic(); // 调用 dynamic 调度并行计算函数
    parallel_static();  // 调用 static 调度并行计算函数
    parallel_guided();  // 调用 guided 调度并行计算函数
    parallel_runtime(); // 调用 runtime 调度并行计算函数
    parallel_auto();    // 调用 auto 调度并行计算函数

    // system("pause");
    return 0;
}

void serial()
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
    end = omp_get_wtime();                                        // 记录串行计算结束时间
    printf("serial matrix multiply time: %0.6lf\n", end - start); // 打印串行计算时间
}

// 进行 dynamic 调度并行计算
void parallel_dynamic()
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

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

    // 打印 dynamic 并行化矩阵乘法的运行时间
    printf("parallel_dynamic matrix multiply time: %0.6lf\n", end - start);
}

// 进行 static 调度并行计算
void parallel_static()
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

#pragma omp parallel shared(a, b, c) private(j, k)
    {
#pragma omp for schedule(static)
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

    // 打印 static 并行化矩阵乘法的运行时间
    printf("parallel_static matrix multiply time: %0.6lf\n", end - start);
}

// 进行 guided 调度并行计算
void parallel_guided()
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

#pragma omp parallel shared(a, b, c) private(j, k)
    {
#pragma omp for schedule(guided)
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

    // 打印 guided 并行化矩阵乘法的运行时间
    printf("parallel_guided matrix multiply time: %0.6lf\n", end - start);
}

// 进行 auto 调度并行计算
void parallel_auto()
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

#pragma omp parallel shared(a, b, c) private(j, k)
    {
#pragma omp for schedule(auto)
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

    // 打印 auto 并行化矩阵乘法的运行时间
    printf("parallel_auto matrix multiply time: %0.6lf\n", end - start);
}

// 进行 runtime 调度并行计算
void parallel_runtime()
{
    double start, end;
    int i, j, k;

    // 获取开始时间
    start = omp_get_wtime();

#pragma omp parallel shared(a, b, c) private(j, k)
    {
#pragma omp for schedule(runtime)
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

    // 打印 runtime 并行化矩阵乘法的运行时间
    printf("parallel_runtime matrix multiply time: %0.6lf\n", end - start);
}