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

#define APK_NOT_END 0x00
#define APK_END 0x01
#define APK_CONFIRM 0x02

typedef struct sockaddr SA;

//数据包分别结构
typedef struct apk_buf {
	int status;
	int number;
	char only_number[16];	//数据唯一编号,struct send_queue 的编号一样,用于服务器确认接受到的什么数据
	int verify;
	int size;
	char buf[TCP_APK_SIZE + 1];
} apk_buf_t;

int tcp_send(int fd, void *, int size);

#if COMPILE_TYPE == 0x00
#include<event.h>


typedef struct accepts_event {
	int cfd;
	int status;
	int size;
	struct event *evt;
	char *recv_buf;
} server_accept_t;

typedef struct server_base {
	int max_connect;
	int connect_num;	//服务器连接数据
	struct event_base*base;
	struct event*ev_listen;
	void* (*new_accept)(int cfd);
	void* (*abnormal)(int cfd);
	void* (*read_call)(void **recv_buf);
	struct thread_pool *thread_pool;
	void *arg;
	char * conencts_info;
} server_base_t;


void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);

struct server_base* tcp_server_init(int port, int listen_num, int max_connect, void *arg);

int tcp_server_start(struct server_base*, struct thread_pool *, void* (*new_accept)(int cfd), void* (*abnormal)(int cfd), void* (*read_call)(void **recv_buf));

void tcp_server_end(struct server_base**);
#else
typedef struct client_base {
	int sfd;
	char *ip;
	int port;
	struct thread_pool *thread_pool;
	void* (*abnormal)(int cfd);
	void* (*read_call)(void **recv_buf);
	void *arg;
} client_base_t;

struct client_base * tcp_client_init(const char *ip, int port);
int tcp_client_start(struct client_base *, void* (*abnormal)(int cfd), void* (*read_call)(void **recv_buf));

#endif


#endif
































