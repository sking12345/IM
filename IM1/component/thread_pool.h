#ifndef THREAD_POLL_H
#define THREAD_POLL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


/**
 * 线程池操作,无锁线程池
 * https://github.com/xhjcehust/LFTPool
 * https://www.cnblogs.com/zhangzl/p/4519976.html
 * https://github.com/xhjcehust/LFTPool
 */


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
	int number;	//线程编号
} thread_t;

typedef struct thread_pool  //线程池队列
{
	struct thread * thread_queue;	//线程池
	int thread_num;		//线程数
} thread_pool_t;

/**
 * [thread_pool_init 初始化]
 * @param  thread_num    [线程数目]
 * @param  queue_max_num [最大任务个数]
 * @return              成功：线程池地址 失败：NULL
 */
struct thread_pool* thread_pool_init(int thread_num, int queue_max_num);

/**
 * 线程池销毁时是否立即关闭线程
 */
void set_destroy_stop(struct thread_pool*);
/**
 * [threadpool_add_job 初始化]
 * @param  thread_pool   [thread_pool_init 初始化返回的值]
 *     [in] callback_function     回调函数
       [in] arg                     回调函数参数
 * @param  queue_max_num [最大任务个数]
 * @return             成功:1 失败:-1
 */

int thread_pool_add_job(struct thread_pool* pool, void* (*callback_function)(void *arg), void *arg);

/**
 * [threadpool_destroy 销毁线程池]
 * @param  pool    [线程数目]
 * @return               [description]
 */
int thread_pool_destroy(struct thread_pool **pool);	//


/**
 * [threadpool_function  线程池中线程函数]
 * @param arg [线程地址]
 */
void* thread_pool_function(void* arg); // 线程池中线程函数



#endif























