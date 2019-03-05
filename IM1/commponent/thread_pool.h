#ifndef THREAD_POOL_H
#define THREAD_POOL_H

//线程池

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct thread_job
{
	void* (*callback_function)(void *arg);
	int arg_size;
	struct thread_job   *next;
	char                buf[0];
} thread_job_t;

typedef struct thread
{

	pthread_t    tid;	//现场ID
	int          queue_max_num;
	struct thread_job *head;
	struct thread_job *tail;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int queue_cur_num;                //队列当前的job个数
	int queue_close;                  //队列是否已经关闭
	int pool_close;                   //线程池是否已经关闭
	int number;	//线程编号
} thread_t;

typedef struct thread_pool  //线程池队列
{
	struct thread * thread_queue;	//线程池
	int thread_num;		//线程数
	int destroy_type;
} thread_pool_t;


struct thread_pool* thread_pool_init(int thread_num, int queue_max_num);
int thread_pool_add_job(struct thread_pool* pool, void* (*callback_function)(void *arg), void *arg, int arg_size);
int thread_pool_destroy(struct thread_pool **pool);
void* thread_pool_function(void* arg); // 线程池中线程函数

#endif












