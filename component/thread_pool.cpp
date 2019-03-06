#include "thread_pool.h"



void* thread_pool_function(void* arg) {
	struct thread *thread = (struct thread*)arg;
	struct thread_job *pjob = NULL;
	while (1)
	{
		pthread_mutex_lock(&(thread->mutex));
		if (thread->queue_cur_num == 0 && thread->pool_close == 0)
		{
			pthread_cond_wait(&(thread->cond), &(thread->mutex));
		}
		if (thread->pool_close)   //线程池关闭，线程就退出
		{
			if (thread->queue_cur_num > 0)
			{

				struct thread_job *node = NULL;
				pjob = thread->head;
				while (pjob != NULL)
				{
					node = pjob;
					pjob = pjob->next;
					free(node);
					node = NULL;
				}
				thread->head = thread->tail = NULL;
			}
			pthread_mutex_unlock(&(thread->mutex));
			pthread_cond_signal(&(thread->exit_cond));

			pthread_exit(NULL);
		}
		thread->queue_cur_num--;
		pjob = thread->head;
		if (thread->queue_cur_num == 0)
		{
			thread->head = thread->tail = NULL;
		}
		else
		{
			thread->head = pjob->next;
		}
		pthread_mutex_unlock(&(thread->mutex));
		(*(pjob->callback_function))(&pjob->buf);   //线程真正要做的工作，回调函数的调用
		free(pjob);
		pjob = NULL;
	}
	return NULL;
}


struct thread_pool* thread_pool_init(int thread_num, int queue_max_num)
{

	struct thread_pool * pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
	if (pool == NULL)
	{
		return NULL;
	}
	pool->thread_num = thread_num;
	pool->thread_queue = (struct thread*)malloc(sizeof(struct thread) * thread_num);
	for (int i = 0; i < thread_num; i++)
	{

		pool->thread_queue[i].queue_max_num = queue_max_num;
		pool->thread_queue[i].head = NULL;
		pool->thread_queue[i].tail = NULL;
		pool->thread_queue[i].queue_cur_num = 0;
		pool->thread_queue[i].queue_close = 0;
		pool->thread_queue[i].pool_close = 0;
		pool->thread_queue[i].number = i;
		if (pthread_mutex_init(&(pool->thread_queue[i].mutex), NULL))
		{
			printf("failed to init mutex!\n");
			break;
		}
		if (pthread_cond_init(&(pool->thread_queue[i].cond), NULL))
		{
			printf("failed to init cond!\n");
			break;
		}
		pthread_create(&(pool->thread_queue[i].tid), NULL, thread_pool_function, (void *) & (pool->thread_queue[i]));
	}
	return pool;
}

void set_destroy_stop(struct thread_pool* pool)
{
	pool->destroy_type = 0x01;

}

int thread_pool_add_job(struct thread_pool* thread_pool, void* (*callback_function)(void *arg), void *arg, int arg_size)
{
	assert(thread_pool != NULL);
	assert(callback_function != NULL);
	assert(arg != NULL);
	struct thread *thread = &(thread_pool->thread_queue[0]);
	for (int i = 0; i < thread_pool->thread_num; ++i)
	{
		if (thread_pool->thread_queue[i].queue_cur_num < thread->queue_cur_num)
		{
			thread = &(thread_pool->thread_queue[i]);
		}

	}
	pthread_mutex_lock(&(thread->mutex));
	if (thread->queue_max_num <= thread->queue_cur_num)
	{
		//printf("%s\n", "queue_max_num <= queue_cur_num");
		//通知管理员的操作
	}
	struct thread_job *pjob = (struct thread_job*) malloc(sizeof(struct thread_job) + arg_size + 1);
	if (NULL == pjob)
	{
		pthread_mutex_unlock(&(thread->mutex));
		return -1;
	}
	pjob->callback_function = callback_function;
	memset(pjob->buf, 0x00, arg_size + 1);
	memcpy(pjob->buf, arg, arg_size);

	pjob->arg_size = arg_size;
	pjob->next = NULL;
	thread->queue_cur_num++;
	if (thread->head == NULL)
	{
		thread->head = thread->tail = pjob;
		pthread_cond_signal(&(thread->cond));
	} else {
		thread->tail->next = pjob;;
		thread->tail = pjob;
	}
	pthread_mutex_unlock(&(thread->mutex));
	return 0;
}

int thread_pool_destroy(struct thread_pool **pool)
{
	assert(*pool != NULL);
	for (int i = 0; i < (*pool)->thread_num; i++)
	{
		struct thread *thread = &((*pool)->thread_queue[i]);
		pthread_mutex_lock(&(thread->mutex));
		if (thread->queue_close || thread->pool_close)
		{
			pthread_mutex_unlock(&(thread->mutex));
			return -1;
		}
		thread->queue_close = 1;
		if ((*pool)->destroy_type != 0x01)
		{
			thread->pool_close = 1;

		} else {
			while (thread->queue_cur_num >= 0)
			{

				pthread_cond_wait(&(thread->exit_cond), &(thread->mutex));  //等待队列为空
			}
		}
		thread->pool_close = 1;
		pthread_mutex_unlock(&(thread->mutex));
		pthread_cond_signal(&(thread->cond));
		pthread_join(thread->tid, NULL);
		pthread_mutex_destroy(&(thread->mutex));
		pthread_cond_destroy(&(thread->cond));
	}
	free((*pool)->thread_queue);
	(*pool)->thread_queue = NULL;
	free(*pool);
	*pool = NULL;
	return 0;
}





