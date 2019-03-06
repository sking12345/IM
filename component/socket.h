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
#include "thread_pool.h"
#include <iostream>
using namespace std;
#define SEND_QUEUE_MAX 1000
#define APK_SIZE 1024

typedef struct apk {	//
	int size;	//数据大小
	int number;	//第多少次数据包
	int status;	//状态 0x00:未接受完,0x01:数据接受完毕,0x02:确认接受完毕数据
	int verify;	//检验数据包是否正常
	char buf[APK_SIZE];
} apk_t;

typedef struct apk_list_buf {	//用于接收数据
	char *data_buf;
} apk_list_buf_t;


typedef struct send_buf
{
	int size;
	int status;
	char *buf;
} send_buf_t;

typedef struct send_buf_list
{
	list<struct send_buf> send_list;
} send_list_t;

typedef struct server_fds {
	int fd;
	int time;
	char status;	//状态,是否在规定时间内验证连接,0x00,未验证 0x01:验证通过
	void *args;
} server_fds_t;


typedef struct tcp_server {
	int status;
	struct event* ev_listen;
	struct event_base* evt_base;
	struct thread_pool* pool;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)(int fd);
	list<struct server_fds> fds;
	map<int, struct apk_list_buf> rec_buf_map;
	map<int, struct send_buf_list> send_buf_map;	//发送数据对列
} tcp_server_t;

typedef struct tcp_client {
	int socket_fd;
	int last_time;
	int status;
	pthread_t pid;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)(int fd);
	map<int, struct send_buf_list> send_buf_map;	//发送数据对列
	struct thread_pool* pool;
} tcp_client_t;


int get_send_buf_size(int fd);	//获取发送缓冲区大小
/**
 * [set_nagle //设置tcp 的 nagle算法]
 * 如果不禁用,则数据会大于1460(tcp) udp(1280) 才发送,会出现异常粘包现象
 * @param  fd     [description]
 * @param  on [ 若为1, 就会在套接字上禁用Nagle算法 ]
 * @return        [description]
 */
int set_nagle(int fd,  int on);
int tcp_server_init(int port, int listen_num);
int tcp_server_start(int port, int listen_num);
int tcp_server_send(int fd, char *buf, int size);
int tcp_server_end();

int tcp_client_init(const char*ip, int port);
int tcp_client_start(int cfd);
int tcp_client_end(int cfd);
int tcp_client_send(int fd, char *buf, int size);

#endif

















