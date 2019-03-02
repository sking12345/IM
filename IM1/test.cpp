
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>

using namespace std;


typedef struct thread_job
{
	void* (*callback_function)(void *arg);
	void                *arg;
	struct thread_job   *next;
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
} thread_t;

typedef struct thread_pool  //线程池队列
{
	struct thread *thread_queue;	//线程池
	int thread_num;		//线程数
} thread_pool_t;

void* thread_pool_function(void* arg)
{
	struct thread *thread = (struct thread*)arg;
	struct thread_job *pjob = NULL;
	while (1)
	{
		pthread_mutex_lock(&(thread->mutex));
		if (thread->queue_cur_num == 0)
		{
			pthread_cond_wait(&(thread->cond), &(thread->mutex));
		}
		printf("%s\n", "ddd");
		thread->queue_cur_num--;
		pthread_mutex_unlock(&(thread->mutex));
	}
	return NULL;
}


struct thread_pool* thread_pool_init(int thread_num, int queue_max_num)
{
	struct thread_pool* poll = (struct thread_pool*)malloc(sizeof(struct thread_pool));
	poll->thread_num = thread_num;
	poll->thread_queue = (struct thread *)malloc(sizeof(struct thread));
	for (int i = 0; i < thread_num; ++i)
	{
		poll->thread_queue[i].queue_max_num = queue_max_num;
		poll->thread_queue[i].queue_cur_num = 0;
		poll->thread_queue[i].head = NULL;
		poll->thread_queue[i].tail = NULL;
		if (pthread_mutex_init(&(poll->thread_queue[i].mutex), NULL))
		{
			printf("failed to init mutex!\n");
			break;
		}
		if (pthread_cond_init(&(poll->thread_queue[i].cond), NULL))
		{
			printf("failed to init queue_empty!\n");
			break;
		}
		poll->thread_queue[i].queue_close = 0;
		poll->thread_queue[i].pool_close = 0;
		pthread_create(&(poll->thread_queue[i].tid), NULL, thread_pool_function, (void *)&poll->thread_queue[i]);
	}
	return poll;
}

int thread_pool_add_job(struct thread_pool* pool, void* (*callback_function)(void *arg), void *arg)
{
	for (int i = 0; i < pool->thread_num; ++i)
	{
		struct thread *thread = &(pool->thread_queue[i]);
		thread->queue_cur_num++;
		pthread_cond_signal(&(thread->cond));
	}
	return 0;
}


int thread_pool_destroy(struct thread_pool *pool)
{
	return 0;
}

void* work(void* arg)
{
	char *p = (char*) arg;
	printf("threadpool callback fuction : %s.\n", p);
	return NULL;
}


int main()
{
	struct thread_pool* poll = thread_pool_init(1, 10);
	thread_pool_add_job(poll, work, (void*)"1");
	while (1);
	return 0;
}


















