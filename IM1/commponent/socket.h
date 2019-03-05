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
#include "thread_pool.h"
using namespace std;

#define SERVER_THREAD_NUM 1
#define CLIENT_THREAD_NUM 1
#define QUEUE_NUM 100


typedef struct client_msg_list
{
	int time;
	int number;
	int send_fd;
	int buf_size;
	char buf[0];
} client_msg_list;

typedef struct server_msg_list
{
	list<struct client_msg_list*> send_msg_list;
} server_msg_list_t;

typedef struct tcp_server {
	struct event* ev_listen;
	struct event_base* evt_base;
	message_base *msg_obj;
	struct thread_pool *pool;

	map<int, struct apk_list> apk_list_map;
	map<int, struct server_msg_list> msg_list_map;

} tcp_server_t;

typedef struct tcp_client
{
	int socket_fd;
	int last_time;
	pthread_t pid;
	struct thread_pool *pool;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)();
	list<struct client_msg_list*> send_msg_list;

} tcp_client_t;



int send_msg(int fd, int number);

int server_send_msg(int fd, char * buf, int size);
int client_send_msg(char * buf, int size);

int tcp_server_init(int port, int listen_num);
int  tcp_server_start(int port, int listen_num, message_base*);
void tcp_server_read(int fd, short events, void *arg);
void tcp_server_accept(int fd, short events, void* arg);
int set_server_thread_pool(struct thread_pool **pool);
void tcp_server_end();

int tcp_client_start(const char *IP, int port);
void* thread_function(void* arg);
int set_client_call_function(int sfd, void* (*msg_call_function)(void *arg), void* (*err_call_function)());
int set_client_thread_pool(struct thread_pool **pool);
void tcp_client_end();


class event_sockt
{
private:
	event_sockt();
	list<struct client_msg_list*> send_msg_list;
public:
	static pthread_mutex_t mutex;
	static event_sockt * get_instance();
	~event_sockt();



};


#endif








