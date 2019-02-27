#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>
#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<event.h>
#include<event2/util.h>
#include <map>
#include <list>
#include "type.h"
#include <iostream>
using namespace std;

class singleton
{
private:
	singleton();
	static singleton * ptr;
	map <int, struct apk_list> apk_list_map;
	map <int, struct connect_list> client_fd_map;
	int listener_fd;	//socket

public:
	static pthread_mutex_t mutex;
	static singleton * get_instance();
	void new_connect(int sfd);	//信息的连接
	void read_connect(int sfd, struct data_apk *);
	void set_listener(int listener_fd);
	void abnormal(int sfd);	//异常

};


void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);
int tcp_server_init(int port, int listen_num);
int tcp_server_start(int port, int listen_num);




#endif


























