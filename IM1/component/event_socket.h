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
#include "types.h"
#include "message_base.h"
#include "thread_pool.h"
#include <iostream>
using namespace std;

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);
int tcp_server_init(int port, int listen_num);
int tcp_server_start(int port, int listen_num, message_base*);
int socket_send(int cfd, char *buf, int size);

void* send_work(void* arg);	//线程调用参数


typedef struct send_queue	//消息发送队列
{
	int cfd;
	int size;
	list<char*> queue_list;
} send_list_t;


//定义基础类
class event_sockt
{
private:
	event_sockt();
	static event_sockt * ptr;
	map <int, struct apk_list> apk_list_map;
	map <int, struct fds_list> fds_list;
	int listener_fd;	//socket
	struct event* ev_listen;
	message_base* msg_obj;
public:
	static pthread_mutex_t mutex;
	static event_sockt * get_instance();
	~event_sockt();
	void new_connect(int cfd, struct sockaddr_in* client);	//信息的连接
	void read_connect(int cfd, struct data_apk );
	void set_listener(int listener_fd);
	void abnormal(int cfd);	//异常
	void set_listener_event(struct event** ev_listen);	//记录tcp_server_start 时的event_new 资源
	void set_msg_obj(message_base* msg_obj);
	message_base* get_msg_obj();
	void socket_send(int cfd, char *buf, int size);


};

#endif












