#ifndef THREAD_POLL_H
#define THREAD_POLL_H

#include <pthread.h>

/**
 * 线程池操作,无锁线程池
 * https://github.com/xhjcehust/LFTPool
 * https://www.cnblogs.com/zhangzl/p/4519976.html
 * https://github.com/xhjcehust/LFTPool
 */


typedef struct thread_job
{
	void               (*routine)(void *);
	void                *arg;
	struct tpool_work   *next;
} thread_job_t;

typedef struct thread
{
	pthread_t    id;	//现场ID
	int          queue_max_num;
	struct thread_job *head;
	pthread_mutex_t mutex;
	pthread_cond_t queue_empty;
	pthread_cond_t queue_not_empty;
	pthread_cond_t queue_not_full;
	int queue_cur_num;                //队列当前的job个数
	int queue_close;                  //队列是否已经关闭
	int pool_close;                   //线程池是否已经关闭
} thread_t;

typedef struct thread_poll   //线程池队列
{
	struct thread * thread;	//线程池
	int thread_num;		//线程数
} thread_poll_t;

#endif













