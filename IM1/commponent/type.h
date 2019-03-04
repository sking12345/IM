#ifndef TYPE_H
#define TYPE_H

#include <map>
#include <list>
#include <iostream>
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
	uint8 type;		//类型 0x01 新消息，0x02 确认消息
	uint8 status;	//状态
	uint32 number;	//编号
	uint32 size;	//数据总大小
	uint8 buf[APK_SIZE + 1];	//数据
} data_apk_t;

typedef struct apk_list {
	int socket_fd;
	int time;
	list<struct  data_apk> list;
} apk_list_t;

//一对多信息交互

typedef struct msg_infos {
	char *buf;

};

#endif