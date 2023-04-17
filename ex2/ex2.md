# 多核环境下OpenMP并行编程

## Hello OpenMP

```c++
#include <stdio.h>
#include <omp.h>    // 包含OpenMP库

int main() {
    int nthreads, tid;  // 定义变量nthreads和tid
    // omp_set_num_threads(8);  // 设置并行区的线程数为 8

    #pragma omp parallel private(tid)  // 并行执行以下代码块，变量tid是每个线程的私有变量
    {
        tid = omp_get_thread_num();  // 获取当前线程的编号
        printf("Hello World from thread %d\n", tid);  // 输出当前线程的编号

        #pragma omp barrier  // 在所有线程都到达这里之前，等待所有线程完成

        if (tid == 0) {
            nthreads = omp_get_num_threads();  // 获取并行区域中的线程数
            printf("There are %d threads\n", nthreads);  // 输出线程数
        }
    }

    return 0;  // 返回0表示程序正常结束
}
```