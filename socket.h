#ifndef _SOCKET_H_
#define _SOCKET_H_
#include "config.h"
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include "log.h"
#include "thread_pool.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
// #include "map.h"
#if COMPILE_TYPE == 0x00
#include<event.h>
#endif

#define APK_NOT_END 0x00
#define APK_END 0x01
#define APK_CONFIRM 0x02

//数据包分别结构
typedef struct apk_buf {
	int status;
	int number;
	char only_number[16];	//数据唯一编号,struct send_queue 的编号一样,用于服务器确认接受到的什么数据
	int verify;
	int size;
	char buf[TCP_APK_SIZE];
} apk_buf_t;



#if TCP_QUEEU_TYPE == 0x01


//数据发送队列
typedef struct send_queue {
	int fd;
	int size;
	int send_number;	//已发送后的编号
	char only_number[16];	//数据唯一编号
	struct  send_queue *next;
	char buf[0];
} send_queue_t;
typedef struct send_thread {
	pthread_mutex_t mutex; //互斥信号量
	pthread_cond_t cond;	//条件变量
	pthread_cond_t close_cond;
	int close;
	int queue_num; //发送queue大小
	struct send_queue *sq_head;
	struct send_queue *sq_tail;
} send_thread_t;
typedef struct send_thread sended_queue_t;
#endif


#if COMPILE_TYPE == 0x00
/**
 * 读取到数据
 */
#if TCP_QUEEU_TYPE == 0x01
typedef struct cond_recv {
	int cfd;
	int status;	//状态是否已分配内存
	char **buf;
} cond_recv_t;
#endif
typedef struct server_read {
	int fd;
	int data_size;
	struct event *ev;
	void **data_buf;
	void *arg;
} server_read_cd_t;

typedef struct server_base {
	int connect_num;	//服务器连接数据
	struct event_base* base;
	struct event* ev_listen;
	void* (*new_accept)(int cfd);
	void* (*abnormal)(int cfd);
	void* (*read_call)(void *sread);
	struct thread_pool *thread_pool;
#if TCP_QUEEU_TYPE == 0x01
	struct send_thread *sthread;
	sended_queue_t * sended_queue;
	char *cond_recv;
#endif
	void *arg;
} server_base_t;

typedef struct server_accept {
	struct event *ev;
	struct server_base *pserver;
} server_accept_t;

struct server_accept * server_accept_new();
void server_accept_free(struct server_accept**);

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);

struct server_base* tcp_server_init(int port, int listen_num);
void set_server_thread_pool(struct thread_pool*);	//设置线程池
void set_server_arg(struct server_base*, void*arg);	//设置处理线程池


void set_server(struct server_base*, struct thread_pool *, void*arg);	//设置处理线程池
//设置回调
void set_server_call(struct server_base* pserver, void* (*new_accept)(int cfd),
                     void* (*read_call)(void *sread),
                     void* (*abnormal)(int cfd));
int get_server_read_fd(void *sread);
void* get_server_read_buf(void *sread);
void fee_server_read_buf(void *sread);
int get_server_read_size(void *sread);


int tcp_server_start(struct server_base*, int thread_num);
int tcp_server_send(int fd, void *, int size, int priority);
void tcp_server_end(struct server_base**);


#else

typedef struct client_read {
	int fd;
	int data_size;
	void *data_buf;

	void *arg;
} client_read_t;



typedef struct client_base {
	int fd;
	char *ip;
	int port;
	struct thread_pool *thread_pool;
	void* (*read_call)(void *cread);
#if TCP_QUEEU_TYPE == 0x01
	struct send_thread *sthread;
	sended_queue_t * sended_queue;
#endif
	void *arg;
} client_base_t;

struct client_base * tcp_client_init(const char *ip, int port);
void set_client_thread_pool(struct client_base*, struct thread_pool *pool, void* (*read_call)(void *cread));
int tcp_client_start(struct client_base*);
/**
 * priority:优先模式,先发送优先级别高的,0 立即发送
 *
 */
#if TCP_QUEEU_TYPE == 0x01
int tcp_client_send(struct client_base*, char *buf, int size, int priority);	//发送数据函数
#else
int tcp_client_send(struct client_base*, char *buf, int size);	//发送数据函数
#endif
int  get_client_fd(void *cread);
char* get_client_read_buf(void *cread);
int  get_client_read_size(void *cread);

void tcp_client_end(struct client_base **);

#endif






#endif









