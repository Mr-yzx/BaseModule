#include "threadpool.h"

/**
/*@brief 线程池任务处理线程
/*
/*@param args 参数
/*@return void* 
*/
static void* process_task_thread(void *args)
{
    if (!args) return NULL;
    threadpool_t *pool = (threadpool_t *)args;
    struct list_head *pos = NULL;
    task_t *task = NULL;

    while (true)
    {
        pthread_mutex_lock(&pool->mutex);
        while (list_empty(&pool->tlist) && !pool->exit)
        {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        if (pool->exit)
        {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        pos = pool->tlist.next_ptr;   //从任务链表取出头结点
        --pool->cur_task_num;
        list_delete_entry(pos);       //从链表中删除
        pthread_mutex_unlock(&pool->mutex);

        task = list_entry(pos, task_t, node);   //从链表节点取出任务节点
        task->func(task->args);
        free(task);
    }

    return NULL;
    
}
threadpool_t *create_threadpool(int thread_nums, int max_task_nums)
{
    if (thread_nums <= 0)
    {
        printf("thread_nums < 0\n");
        return NULL;
    }

    threadpool_t *pool = (threadpool_t *)malloc(sizeof(threadpool_t));
    if (!pool) 
    {
        printf("malloc threadpool_t fail\n");
        return NULL;
    }
    memset(pool, 0, sizeof(threadpool_t));

    init_list_head(&pool->tlist);

    pool->cur_task_num = 0;
    pool->thread_num = thread_nums;
    pool->max_task_num = max_task_nums;
    pool->exit = 0;

    pool->thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * thread_nums);
    if (!pool->thread_ids)
    {
        printf("malloc thread_ids fail\n");
        return NULL;
    }

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < pool->thread_num; ++i)
    {
        pthread_create(&pool->thread_ids[i], NULL, process_task_thread, pool);
    }

    return pool;
}

int destroy_threadpool(threadpool_t *pool)
{
    if (!pool)
    {
        printf("pool is NULL\n");
        return -1;
    }
    while (!list_empty(&pool->tlist))     //等待任务链表为空
    {
        continue;
    }
    pool->exit = 1;
    pthread_cond_broadcast(&pool->cond);  //唤醒所有线程

    for(int i = 0; i < pool->thread_num; ++i)   //等待所有线程函数执行完毕
    {
        pthread_join(pool->thread_ids[i], NULL);
    }

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    
    free(pool->thread_ids);
    free(pool);
    pool = NULL;

    return 0;
}

int add_task_threadpool(threadpool_t *pool, task_func_t func, void *args, int priority)
{
    if (!pool) return -1;
    if (!func) return -2;
    if (pool->cur_task_num > pool->max_task_num) return -3;

    task_t *task = (task_t *)malloc(sizeof(task_t));
    task->func = func;
    task->args = args;

    pthread_mutex_lock(&pool->mutex);
    if (priority == 1)       //高优先级
    {
        list_add_head(&task->node, &pool->tlist);
    }
    else
    {
        list_add_tail(&task->node, &pool->tlist);
    }

    ++pool->cur_task_num;
    pthread_mutex_unlock(&pool->mutex);

    pthread_cond_signal(&pool->cond);   //通知线程取任务

    return 0;
}

int get_task_num_threadpool(threadpool_t *pool)
{
    return pool? pool->cur_task_num : -1;
}
