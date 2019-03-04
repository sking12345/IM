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
#include "type.h"
#include "message_base.h"
using namespace std;

typedef struct tcp_server {
	struct event* ev_listen;
	struct event_base* evt_base;
	message_base *msg_obj;
	map<int, struct apk_list> apk_list_map;
} tcp_server_t;


int  tcp_server_start(int port, int listen_num, message_base*);
void tcp_server_read(int fd, short events, void *arg);
void tcp_server_accept(int fd, short events, void* arg);
void tcp_server_stop();
int send_msg(int fd, char *buf, int size);

#endif








