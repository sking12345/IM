#ifndef TYPE_H
#define TYPE_H

#include <map>
#include <list>
#include <iostream>
using namespace std;
#define DEBUG 0x00
#define APK_SIZE 1


typedef  unsigned char uint8;
typedef  unsigned short uint16;
typedef  unsigned int uint32;
typedef  unsigned long long uint64;

typedef  char int8;
typedef  short int16;
typedef  int int32;
typedef  long long int64;

typedef struct  data_apk	// 定义传输基本数据结构
{
	uint8 version;	//版本
	uint8 type;		//类型
	uint8 status;	//状态
	uint32 number;	//编号
	uint32 size;	//数据总大小
	uint8 buf[APK_SIZE + 1];	//数据
} data_apk_t;

typedef struct apk_list //接受到的数据包数据存入该结构
{
	uint32 sockfd;	//socket 链接
	uint32 number;	//数据编号
	list<struct data_apk> list;
} apk_list_t;

typedef struct fds_list // 记录链接时间,用于心跳检测
{
	uint32 status;	//状态,用于判断是否已通过验证操作
	time_t time;
} fds_list_t;

typedef struct  msg_buf	//数据
{
	uint32 size;	//数据大小
	uint32 number;	//编号
	int8 data[0];
} msg_buf_t;


#endif









