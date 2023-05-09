# 多机环境下MPI并行编程

**魏鹏涛 20120605**

## 实验过程

### 1. 安装MPI

#### 1.1 安装 **MPI** 及其所需的依赖项
```bash
yum install gcc gcc-c++ openmpi-devel
```

#### 1.2 添加环境变量
```bash
export PATH=/usr/lib64/openmpi/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:$LD_LIBRARY_PATH
```

并在```~/.bashrc```文件末尾添加
```bash
export PATH=/usr/lib64/openmpi/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:$LD_LIBRARY_PATH
```

添加后```~/.bashrc```文件如下
```bash

# .bashrc

# User specific aliases and functions

alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'

# Source global definitions
if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi
export PATH=/usr/lib64/openmpi/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:$LD_LIBRARY_PATH
```

```bash
vim ~/.bashrc
source ~/.bashrc
```

#### 1.3 测试是否安装成功
```bash
mpicc --version
```

测试结果如下所示说明安装成功
```bash
gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)
Copyright © 2015 Free Software Foundation, Inc.
本程序是自由软件；请参看源代码的版权声明。本软件没有任何担保；
包括没有适销性和某一专用目的下的适用性担保。
```

### 2. 创建多进程，输出进程号和进程数

#### 2.1 编写程序如下
```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int rank, size; // rank 是当前进程的编号，size 是进程总数

    MPI_Init(&argc, &argv);               // 初始化 MPI 环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取当前进程的编号
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获取进程总数

    printf("Hello from process %d of %d\n", rank, size);

    MPI_Finalize(); // 释放 MPI 资源

    return 0;
}
```

#### 2.2 编译运行（单机）
```bash
mpicc -o hello_mpi hello_mpi.c
mpirun -np 4 hello_mpi # 普通用户
mpirun --allow-run-as-root -np 4 hello_mpi # root 用户
```

结果如下
```bash
[root@centos100 mpi]# mpirun --allow-run-as-root -np 4 hello_mpi
Hello from process 2 of 4
Hello from process 0 of 4
Hello from process 3 of 4
Hello from process 1 of 4
```

#### 2.3 多机运行

首先在当前工作目录创建```hostfile```文件，内容如下
```bash
centos100
centos101
centos102
```
关闭防火墙
```
systemctl stop firewalld
```

运行
```bash
mpirun -np 3 -hostfile hostfile ./hello_mpi # 普通用户
mpirun --allow-run-as-root -np 3 -hostfile hostfile ./hello_mpi # root 用户
```

结果如下
```bash
[root@centos100 mpi]# mpirun --allow-run-as-root -np 3 -hostfile hostfile ./hello_mpi
Hello from process 0 of 3
Hello from process 1 of 3
Hello from process 2 of 3
```

### 3. 编程实现大规模向量/矩阵并行计算

#### 3.1 大规模向量运算

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define VECTOR_SIZE 300

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *x, *y;
    int i, n = VECTOR_SIZE / size;            // n 是每个进程的向量长度
    x = (double *)malloc(n * sizeof(double)); // x 和 y 是每个进程的局部向量
    y = (double *)malloc(n * sizeof(double));

    // 初始化局部向量
    for (i = 0; i < n; i++)
    {
        x[i] = i + rank * n;
        y[i] = i + rank * n + n;
    }

    // 计算局部内积
    double sum = 0.0;
    for (i = 0; i < n; i++)
    {
        sum += x[i] * y[i];
    }

    // 汇总局部内积
    double global_sum;
    MPI_Reduce(&sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 打印结果
    if (rank == 0)
    {
        printf("The dot product of x and y is %.2f\n", global_sum);
    }

    free(x);
    free(y);

    MPI_Finalize();
    return 0;
}
```

运行结果
```bash
[root@centos100 mpi]# mpirun --allow-run-as-root -np 3 vector_mpi
The dot product of x and y is 13494000500.00
[root@centos100 mpi]# mpirun --allow-run-as-root -np 3 --hostfile hostfile --mca btl_tcp_if_include ens33 ./vector_mpi
The dot product of x and y is 13494000500.00
```

实验过程中发现去掉```--mca btl_tcp_if_include ens33```就无法运行，查阅资料发现可能是在这里MPI使用的默认网络网络接口不能通信

#### 3.2 大规模矩阵运算

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 300 // 矩阵大小

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);               // 初始化 MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取进程 ID
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获取进程数量

    if (size < 2)
    { // 至少需要两个进程
        fprintf(stderr, "Error: At least two processes required\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int i, j, k;
    double *matrixA, *matrixB, *matrixC;
    double startTime, endTime;

    // 每个节点分配矩阵
    matrixA = (double *)malloc(N * N / size * sizeof(double));
    matrixB = (double *)malloc(N * N / size * sizeof(double));
    matrixC = (double *)malloc(N * N / size * sizeof(double));

    // 初始化矩阵
    for (i = 0; i < N / size; i++)
    {
        for (j = 0; j < N; j++)
        {
            matrixA[i * N + j] = rank + 1;
            matrixB[i * N + j] = j + 1;
            matrixC[i * N + j] = 0;
        }
    }

    // 等待所有进程完成初始化
    MPI_Barrier(MPI_COMM_WORLD);

    // 计算矩阵乘积
    startTime = MPI_Wtime(); // 记录开始时间
    for (i = 0; i < N / size; i++)
    {
        for (j = 0; j < N; j++)
        {
            for (k = 0; k < N / size; k++)
            {
                matrixC[i * N + j] += matrixA[i * N + k] * matrixB[k * N + j];
            }
        }
    }
    endTime = MPI_Wtime(); // 记录结束时间

    // 等待所有进程完成计算
    MPI_Barrier(MPI_COMM_WORLD);

    // 将结果返回到主节点
    if (rank == 0)
    {
        double *temp = (double *)malloc(N * N * sizeof(double));
        MPI_Gather(matrixC, N * N / size, MPI_DOUBLE, temp, N * N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        printf("Time: %f seconds\n", endTime - startTime);
        free(temp);
    }
    else
    {
        MPI_Gather(matrixC, N * N / size, MPI_DOUBLE, NULL, N * N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // 释放内存并结束 MPI
    free(matrixA);
    free(matrixB);
    free(matrixC);
    MPI_Finalize();
    return 0;
}
```

结果如下
```bash
[root@centos100 mpi]# mpirun --allow-run-as-root -np 3 ./matrix_mpi
Time: 0.139863 seconds
[root@centos100 mpi]# mpirun --allow-run-as-root -np 3 -hostfile hostfile --mca btl_tcp_if_include ens33 ./matrix_mpi
Time: 0.038913 seconds
```