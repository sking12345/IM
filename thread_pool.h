#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct thread_job  //线程任务
{
	void* (*callback_function)(void *arg);    //线程回调函数
	void *arg;                                //回调函数参数
	struct thread_job *next;
} thread_job_t;


typedef struct thread
{
	pthread_t pid; //线程ID
	int close; //是否关闭线程
	int queue_num;	 	//队列数量
	pthread_mutex_t mutex; //互斥信号量
	pthread_cond_t cond;	//条件变量
	pthread_cond_t empty_cond;
	struct thread_job *job_head;
	struct thread_job *job_tail;
} thread_t;

typedef struct thread_pool
{
	int num;	//线程数量
	int max_queue_num;	//线程最大数量
	struct thread *thread_queue;	//线程队列
} thread_pool_t;

struct thread_pool * thread_pool_init(int num, int max_queue_num);
int thread_add_job(struct thread_pool *, void* (*callback_function)(void *arg), void *arg);
int thread_pool_destroy(struct thread_pool **pool);
void* thread_function(void* arg);
#endif