#ifndef SOCKET_H
#define SOCKET_H
#include "config.h"
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#if COMPILE_TYPE == 0x00
#include<event.h>

typedef struct server_t
{
	struct event_base* base;
	struct event* ev_listen;
} SERVER_T;

typedef struct server_accept
{
	struct event *ev;
	struct server_t *pserver;
} SERVER_ACCEPT;


struct server_accept * server_accept_new();
void server_accept_free(struct server_accept**);


void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);

struct server_t* tcp_server_init(int port, int listen_num);

void tcp_server_start(struct server_t*, int thread_num);
void tcp_server_end(struct server_t**);

#else

#endif


#endif