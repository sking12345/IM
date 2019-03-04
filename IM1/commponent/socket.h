#ifndef SOCKET_H
#define SOCKET_H
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
#include<errno.h>
#include <string.h>
#include<event.h>
#include<event2/util.h>
#include <map>
#include <list>
#include <iostream>
#include "message_base.h"
using namespace std;
typedef struct tcp_fds
{
	int socket_fd;
	int last_time;	//最后一次数据通信的时间
	int status;	//状态,用于验证是否正常登录
}tcp_fds_t;

typedef struct tcp_server
{
	map<int,struct tcp_fds> queue_map;
	struct event* ev_listen;
	struct event_base* evt_base;
	message_base *msg_obj;
}tcp_server_t;

int  tcp_server_start(int port, int listen_num);
void tcp_server_read(int fd, short events, void *arg);
void tcp_server_accept(int fd, short events, void* arg);
void tcp_server_stop(struct tcp_server**);
int send_msg(int fd,char *buf,int size);

#endif