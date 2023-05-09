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