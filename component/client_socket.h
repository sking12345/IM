
#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
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
#include "types.h"
#include <pthread.h>
#include "thread_pool.h"


typedef struct socket_info
{
	int socket_fd;
	int time;
	pthread_t pid;
	void* (*msg_call_function)(void *arg);
	void* (*err_call_function)();
} socket_info_t;




int tcp_client_start(const char *IP, int port);
//设置回调函数
struct socket_info * set_call_function(int sfd, void* (*msg_call_function)(void *arg), void* (*err_call_function)());


void send_msg(int fd, char *buf, int size); //send data

void* thread_function(void* arg); // 线程池中线程函数

void tcp_client_end(struct socket_info **);
#endif





