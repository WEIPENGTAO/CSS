#include <iostream>
#include <ctime>
#include <cmath>
#include <omp.h>
using namespace std;

int count_primes_serial(int n)
{
    if (n < 2)
        return 0;
    int count = 0;
    for (int i = 2; i <= n; i++)
    {
        bool is_prime = true;
        for (int j = 2; j <= sqrt(i); j++)
        {
            if (i % j == 0)
            {
                is_prime = false;
                break;
            }
        }
        if (is_prime)
            count++;
    }
    return count;
}

int count_primes_parallel(int n, int num_threads)
{
    if (n < 2)
        return 0;
    int count = 0;
#pragma omp parallel num_threads(num_threads) reduction(+ : count)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int start = (n / nthreads) * tid + 2;
        int end = (n / nthreads) * (tid + 1) + 1;
        if (tid == nthreads - 1)
            end = n + 1;

        for (int i = start; i < end; i++)
        {
            bool is_prime = true;
            for (int j = 2; j <= sqrt(i); j++)
            {
                if (i % j == 0)
                {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime)
                count++;
        }
    }
    return count;
}

int main()
{
    int n = 1e7;
    int num_threads = 8;

    // calculate primes using serial algorithm
    clock_t start_time = clock();
    int count_serial = count_primes_serial(n);
    clock_t end_time = clock();
    double serial_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
    cout << "Serial algorithm: found " << count_serial << " primes in " << serial_time << " seconds" << endl;

    // calculate primes using parallel algorithm
    start_time = clock();
    int count_parallel = count_primes_parallel(n, num_threads);
    end_time = clock();
    double parallel_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
    cout << "Parallel algorithm with " << num_threads << " threads: found " << count_parallel << " primes in " << parallel_time << " seconds" << endl;

    system("pause");
    return 0;
}
