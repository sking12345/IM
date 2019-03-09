#ifndef SOCKET_H
#define SOCKET_H
#include "config.h"
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include "log.h"
#include "thread_pool.h"
#if COMPILE_TYPE == 0x00
#include<event.h>


/**
 * 读取到数据
 */
typedef struct server_read
{
	int fd;
	struct event *ev;
	void *data_buf;
	void *arg;
} server_read_cd_t;

typedef struct server_base {
	struct event_base* base;
	struct event* ev_listen;
	void* (*new_accept)(int cfd);
	void* (*abnormal)(int cfd);
	void* (*new_data)(void *sread);
	struct thread_pool *thread_pool;
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
                     void* (*new_data)(void *sread),
                     void* (*abnormal)(int cfd));
int get_server_read_fd(void *sread);
char* get_server_read_data(void *sread);



int tcp_server_start(struct server_base*, int thread_num);
void tcp_server_end(struct server_base**);

#else

#endif


#endif









