#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

#define DATASIZE 5000000
#define THREADS_NUM 1

// merge sort
vector<long> merge(const vector<long> &left, const vector<long> &right)
{
    vector<long> result;
    unsigned left_it = 0, right_it = 0;
    while (left_it < left.size() && right_it < right.size())
    {
        if (left[left_it] < right[right_it])
        {
            result.push_back(left[left_it]);
            left_it++;
        }
        else
        {
            result.push_back(right[right_it]);
            right_it++;
        }
    }

    while (left_it < left.size())
    {
        result.push_back(left[left_it]);
        left_it++;
    }

    while (right_it < right.size())
    {
        result.push_back(right[right_it]);
        right_it++;
    }

    return result;
}

vector<long> mergesort(vector<long> &vec, int threads)
{
    if (vec.size() == 1)
    {
        return vec;
    }

    vector<long>::iterator middle = vec.begin() + (vec.size() / 2);

    vector<long> left(vec.begin(), middle);
    vector<long> right(middle, vec.end());

    if (threads > 1)
    {
#pragma omp parallel sections
        {
#pragma omp section
            {
                left = mergesort(left, threads / 2);
            }

#pragma omp section
            {
                right = mergesort(right, threads - threads / 2);
            }
        }
    }
    else
    {
        left = mergesort(left, 1);
        right = mergesort(right, 1);
    }
    return merge(left, right);
}

int main(void)
{
    vector<long> v(DATASIZE);

    // 随机生成数据
    srand((unsigned)time(NULL));
    for (long i = 0; i < DATASIZE; ++i)
        v[i] = rand() % 1000000;

    time_t begin_t, end_t;

    begin_t = time(NULL);
    v = mergesort(v, THREADS_NUM);
    end_t = time(NULL);

    ofstream out;
    if (THREADS_NUM > 1)
    {
        out.open("multi_process.out");
    }
    else
    {
        out.open("naive_process.out");
    }

    // for (long i = 0; i < DATASIZE; ++i)
    //     out << v[i] << "\t";
    out << "Merge Sort Finished." << endl;
    out << "Use time: " << difftime(end_t, begin_t) << endl;

    system("pasue");
    return 0;
}
