#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<event.h>
#include<event2/util.h>
#include <map>
#include <list>
#include <iostream>
using namespace std;
#define APK_SIZE 100

typedef  unsigned char uint8;
typedef  unsigned short uint16;
typedef  unsigned int uint32;
typedef  unsigned long long uint64;



typedef struct data_apk	//数据分包数据
{
	uint8 version: 4;   //版本号
	uint8 s_type: 4;   //服务类型
	uint8 ttl;  //生存时间
	char buf[APK_SIZE];
} TCP_APK;

typedef struct apk_list //接受到的数据包数据存入该结构
{
	int socket_fd;	//socket 链接
	int data_size;	//数据大小
	int apk_num;	// 数据包数量
	int recv_num;	//接受数量
	list<struct data_apk> list;
} APK_LIST;

typedef struct message
{
	int socket_fd;
	struct apk_list apk_list;
} MESSAGE;

class singleton
{
public:
	static pthread_mutex_t mutex;
	static singleton * get_instance();

private:
	singleton();
	static singleton * ptr;

};




void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);
int tcp_server_init(int port, int listen_num);
int tcp_server_start(int port, int listen_num);




#endif


























