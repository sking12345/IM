#ifndef SOCKET_H
#define SOCKET_H
#include "config.h"
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include "log.h"
#if COMPILE_TYPE == 0x00
#include<event.h>
#include "thread_pool.h"

typedef struct apk_buf {
	int size;
	int number;
	char buf[TCP_APK_SIZE + 1];
} apk_buf_t;

typedef struct server_base {
	struct event_base* base;
	struct event* ev_listen;
	struct thread_pool *pthread_pool;
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

void set_server_thread_poll(struct server_base*, struct thread_pool*);	//设置处理线程池

int tcp_server_start(struct server_base*, int thread_num);
void tcp_server_end(struct server_base**);

#else

#endif


#endif