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

#define APK_SIZE 1
typedef  unsigned char uint8;
typedef  unsigned short uint16;
typedef  unsigned int uint32;
typedef  unsigned long long uint64;

typedef  char int8;
typedef  short int16;
typedef  int int32;
typedef  long long int64;

typedef struct  data_apk {	// 定义传输基本数据结构
	uint8 version;	//版本
	uint8 type;		//类型
	uint8 status;	//状态
	uint32 number;	//编号
	uint32 size;	//数据总大小
	uint8 buf[APK_SIZE + 1];	//数据
} data_apk_t;

typedef struct tcp_server {
	struct event* ev_listen;
	struct event_base* evt_base;
	message_base *msg_obj;
} tcp_server_t;

int  tcp_server_start(int port, int listen_num, message_base*);
void tcp_server_read(int fd, short events, void *arg);
void tcp_server_accept(int fd, short events, void* arg);
void tcp_server_stop();
int send_msg(int fd, char *buf, int size);

#endif








