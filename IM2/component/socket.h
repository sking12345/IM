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
using namespace std;

#define APK_SIZE 1

typedef struct apk {
	int size;	//数据大小
	int number;	//第多少次数据包
	int status;	//状态 0x00:未接受完,0x01:数据接受完毕
	int verify;	//检验数据包是否正常
	char buf[APK_SIZE];
} apk_t;

typedef struct apk_list {
	struct apk *apk;
	struct apk_list *next;
	struct apk_list *tail;
} apk_list_t;

typedef struct server_fds {
	int fd;
	int time;
	char status;	//状态,是否在规定时间内验证连接,0x00,未验证 0x01:验证通过
	void *args;
} server_fds_t;

typedef struct tcp_server {
	struct event* ev_listen;
	struct event_base* evt_base;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)(int fd);
	list<struct server_fds> fds;
	map<int, struct apk_list*> apk_list_map;
} tcp_server_t;

typedef struct tcp_client {
	int socket_fd;
	int last_time;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)(int fd);
	map<int, struct apk_list> apk_list_map;
} tcp_client_t;

int tcp_server_init(int port, int listen_num);
int tcp_server_start(int port, int listen_num);
int tcp_server_end();
int tcp_client_init(const char*ip, int port);
int tcp_client_start(const char*ip, int port);
int tcp_client_end();
#endif


















