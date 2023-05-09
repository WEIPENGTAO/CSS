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
