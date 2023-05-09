#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define VECTOR_SIZE 3000

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