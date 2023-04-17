#include <omp.h>    // OpenMP的头文件
#include <stdio.h>  // 标准I/O库的头文件
#include <stdlib.h> // 标准库的头文件

int main()
{
    int nthreads, tid;            // 声明整型变量nthreads和tid
    omp_set_num_threads(8);       // 设置使用8个线程并行执行
    // parallel是并行执行的代码块，private是私有变量，即每个线程都有自己的变量副本
    #pragma omp parallel private(nthreads, tid)  // 使用多线程并行执行代码块
    {
        tid = omp_get_thread_num();     // 获取线程ID
        printf("Hello World from OMP thread %d\n", tid);  // 输出线程ID
        if (tid == 0)  // 只有ID为0的线程输出以下内容
        {
            nthreads = omp_get_num_threads();  // 获取线程总数
            printf("Number of threads is %d\n", nthreads);  // 输出线程总数
        }
    }

    system("pause");  // 暂停命令行窗口，等待用户按下任意键
    return 0;         // 返回0表示程序正常结束
}
