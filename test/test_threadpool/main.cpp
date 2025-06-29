#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

typedef struct task_info_t
{
    int times;
    char buff[64];
} task_info_t;

void task1(void *args)
{
    task_info_t *info = (task_info_t *)args;
    printf("handle task1 pid = %lld, times = %d, buffer = %s\n", pthread_self(), info->times, info->buff);
    free(args);
}

void task2(void *args)
{
    task_info_t *info = (task_info_t *)args;
    printf("handle task2 pid = %lld, times = %d, buffer = %s\n", pthread_self(), info->times, info->buff);
    free(args); 
}

void task3(void *args)
{
    task_info_t *info = (task_info_t *)args;
    printf("handle task3 pid = %lld, times = %d, buffer = %s\n", pthread_self(), info->times, info->buff);
    free(args); 
}

void test_threadpool(void)
{
    threadpool_t *pool = create_threadpool(4, 64);

    task_info_t *info;

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            info = (task_info_t *)malloc(sizeof(task_info_t));
            info->times = i;
            sprintf(info->buff, "threadpool task1...");
            add_task_threadpool(pool, task1, info, 1);

            info = (task_info_t *)malloc(sizeof(task_info_t));
            info->times = i;
            sprintf(info->buff, "threadpool task2...");
            add_task_threadpool(pool, task2, info, 0);

            info = (task_info_t *)malloc(sizeof(task_info_t));
            info->times = i;
            sprintf(info->buff, "threadpool task3...");
            add_task_threadpool(pool, task3, info, 0);
        }
        sleep(1);
    }

    destroy_threadpool(pool);
    printf("threadpool test end\n");
}

int main(void)
{
    test_threadpool();
    return 0;
}
