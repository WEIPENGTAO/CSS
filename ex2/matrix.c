/*
这是一个矩阵乘法的并行实现
测试了串行和并行两种实现方式的运行时间
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define m 1000

float a[m][m]; // 矩阵A
float b[m][m]; // 矩阵B
float c[m][m]; // 矩阵C

void serial(); // 串行计算函数
void parell(); // 并行计算函数

int main()
{
    clock_t start, end;

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

    omp_set_num_threads(4);                      // 设置并行线程数为 4
    int pnum = omp_get_num_procs();              // 获取处理器核心数
    fprintf(stderr, "Thread_pnum = %d\n", pnum); // 打印处理器核心数

    serial(); // 调用串行计算函数
    parell(); // 调用并行计算函数

    getchar();
    return 0;
}

void serial()
{
    clock_t start, end;
    int i, j, k;

    start = clock(); // 记录串行计算开始时间
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
    end = clock();                                                                           // 记录串行计算结束时间
    printf("serial matrix multiply time: %0.6lf\n", ((double)end - start) / CLOCKS_PER_SEC); // 打印串行计算时间
}

// 进行矩阵乘法的并行实现
void parell()
{
    clock_t start, end;
    int i, j, k;

    // 获取开始时间
    start = clock();

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
    end = clock();

    // 打印并行化矩阵乘法的运行时间
    printf("parell matrix multiply time: %0.6lf\n", ((double)end - start) / CLOCKS_PER_SEC);
}
