#include "thread_pool.h"




struct thread_pool * thread_pool_init(int num, int max_queue_num)
{
	struct thread_pool * pool = (struct thread_pool *)malloc(sizeof(struct thread_pool));
	pool->num = num;
	pool->max_queue_num = max_queue_num;
	pool->thread_queue = (struct thread *)malloc(sizeof(struct thread) * num);
	for (int i = 0; i < num; i++)
	{
		pthread_cond_init(&(pool->thread_queue[i].cond), NULL);
		pthread_cond_init(&(pool->thread_queue[i].empty_cond), NULL);
		pthread_mutex_init(&(pool->thread_queue[i].mutex), NULL);
		pool->thread_queue[i].close = 0;
		pool->thread_queue[i].queue_num = 0;
		pool->thread_queue[i].job_head = NULL;
		pool->thread_queue[i].job_tail = NULL;
		pthread_create(&(pool->thread_queue[i].pid), NULL, thread_function, (void *)&pool->thread_queue[i]);
	}
	return pool;
}

int thread_add_job(struct thread_pool *pool, void* (*callback_function)(void *arg), void *arg)
{
	if (pool == NULL)
	{
		return -1;
	}
	struct thread *pthread = &(pool->thread_queue[0]);
	pthread_mutex_lock(&(pthread->mutex));
	int queue_num = pthread->queue_num;
	pthread_mutex_unlock(&(pthread->mutex));
	for (int i = 0; i < pool->num; i++)
	{
		struct thread *thread = &(pool->thread_queue[i]);
		pthread_mutex_lock(&(thread->mutex));
		if (pthread->close)
		{
			pthread_mutex_unlock(&(pthread->mutex));
			continue;
		}
		if (thread->queue_num <= queue_num)
		{
			pthread = &(pool->thread_queue[i]);
			queue_num = pthread->queue_num;
		}
		pthread_mutex_unlock(&(thread->mutex));
	}
	pthread_mutex_lock(&(pthread->mutex));
	struct thread_job *job = (struct thread_job*)malloc(sizeof(struct thread_job));
	job->arg = arg;
	job->callback_function = callback_function;
	job->next = NULL;
	if (pthread->job_head == NULL)
	{
		pthread->job_head = pthread->job_tail = job;
	} else {
		pthread->job_tail->next = job;
		pthread->job_tail = job;
	}
	pthread->queue_num++;
	pthread_mutex_unlock(&(pthread->mutex));
	pthread_cond_signal(&(pthread->cond));
	return 0;
}

int thread_pool_destroy(struct thread_pool **pool)
{
	for (int i = 0; i < (*pool)->num; i++)
	{
		struct thread *ptheard = &(*pool)->thread_queue[i];
		pthread_mutex_lock(&(ptheard->mutex));
		if (ptheard->close)
		{
			continue;
		}
		ptheard->close = 1;
		pthread_mutex_unlock(&(ptheard->mutex));
		pthread_cond_signal(&(ptheard->cond));
		while (ptheard->queue_num != 0)
		{
			pthread_cond_wait(&(ptheard->empty_cond), &(ptheard->mutex));
		}
		pthread_mutex_destroy(&(ptheard->mutex));          //清理资源
		pthread_cond_destroy(&(ptheard->empty_cond));
		pthread_cond_destroy(&(ptheard->cond));
	}
	free((*pool)->thread_queue);
	(*pool)->thread_queue = NULL;
	return 0;
}

void* thread_function(void* arg)
{
	struct thread *pthread = (struct thread*)arg;
	struct thread_job * job = NULL;
	while (1)
	{
		pthread_mutex_lock(&(pthread->mutex));
		while (pthread->queue_num == 0)   //队列为空时，就等待队列非空
		{

			pthread_cond_wait(&(pthread->cond), &(pthread->mutex));
		}
		if (pthread->close)	//关闭线程
		{
			if (pthread->queue_num > 0)
			{
				struct thread_job * node = NULL;
				struct thread_job * head = pthread->job_head;
				while (head != NULL)
				{
					node = head;
					head = head->next;
					free(node);
					node = NULL;
					pthread->queue_num--;
				}
			}
			pthread_mutex_unlock(&(pthread->mutex));
			pthread_cond_signal(&(pthread->empty_cond));
			pthread_exit(NULL);
		}
		if (pthread->job_head != NULL)
		{
			job = pthread->job_head;
			pthread->job_head = pthread->job_head->next;
			pthread->queue_num--;
		}
		pthread_mutex_unlock(&(pthread->mutex));
		if (job != NULL)
		{
			(*(job->callback_function))(job->arg);   //线程真正要做的工作，回调函数的调用
			free(job);
			job = NULL;
		}
	}
}















