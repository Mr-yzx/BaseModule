#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <limits.h>
#include "list.h"


/**
/*@brief 任务回掉函数
/*
*/
typedef void (*task_func_t)(void *args);


/**
/*@brief 任务节点结构体
/*
*/
typedef struct task_t
{
    struct list_head      node;
    task_func_t           func;
    void                  *args;
} task_t;

/**
/*@brief 线程池结构体
/*
*/
typedef struct threadpool_t
{
    int                   thread_num;    //线程池数量
    int                   max_task_num;  //最大任务数量
    volatile int          cur_task_num;  //当前线程池任务数量
    volatile int          exit;          //线程池退出标志
    struct list_head      tlist;         //任务链表
    pthread_mutex_t       mutex;         //互斥锁
    pthread_cond_t        cond;          //条件变量
    pthread_t             *thread_ids;   //线程池数组       
} threadpool_t;


/**
/*@brief 创建线程池
/*
/*@param thread_nums 线程数量
/*@param max_task_nums 最大任务数量
/*@return threadpool_t* 线程池句柄
 */
threadpool_t* create_threadpool(int thread_nums, int max_task_nums);

/**
/*@brief 销毁线程池
/*
/*@param pool 线程池句柄
/*@return int 
 */
int destroy_threadpool(threadpool_t *pool);

/**
/*@brief 添加任务到线程池
/*
/*@param pool 线程池句柄
/*@param func 任务函数
/*@param args 任务参数
/*@param priority 任务优先级
/*@return int 
 */
int add_task_threadpool(threadpool_t *pool, task_func_t func, void *args, int priority);

/**
/*@brief 获取线程池任务数量
/*
/*@param pool 线程池句柄
/*@return int 任务数量
 */
int get_task_num_threadpool(threadpool_t *pool);

#endif /* __THREADPOOL_H__ */