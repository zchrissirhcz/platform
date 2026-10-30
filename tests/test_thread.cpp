#include "platform/thread.hpp"

#include <stdio.h>
#include <memory>

#define TH_NUM 4
#define N 100

static void* thread_increment(void* array)
{
    int i;
    int* iptr;

    iptr = (int*)array;
    for (i=0; i<N/TH_NUM; i++)
    {
        iptr[i] += 1;
    }
    return NULL;
}

static void print_array(int* array)
{
    for (int i=0; i<N; i++)
    {
        if (i > 0 && i % 10 == 0)
        {
            printf("\n");
        }
        printf("%d, ", array[i]);
    }
    printf("\n");
}

int main()
{
    int i;
    std::shared_ptr<Thread> thread[TH_NUM];
    int array[N];

    for (i=0; i<N; i++)
    {
        array[i] = i;
    }
    printf("before:\n");
    print_array(array);

    for (i=0; i<TH_NUM; i++)
    {
        thread[i] = std::make_shared<Thread>(thread_increment, array + i * N / TH_NUM);
    }

    for (i=0; i<TH_NUM; i++)
    {
        thread[i]->join();
    }

    printf("after:\n");
    print_array(array);

    return 0;
}