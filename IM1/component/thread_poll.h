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
	int          shutdown;
	struct thread_job *head;

} thread_t;

#endif