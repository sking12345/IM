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


typedef struct test_apk
{
	uint32 test;
	char buf[0];

} TEST_APK;



typedef struct data_apk	//数据分包数据
{
	uint8 version: 4;   //版本号
	uint8 s_type: 4;   //服务类型
	uint8 status;	//数据包状态
	uint32 number;	//数据包编号
	uint32 size;
	uint8 buf[APK_SIZE];
} TCP_APK;

typedef struct apk_list //接受到的数据包数据存入该结构
{
	uint32 sockfd;	//socket 链接
	uint32 number;	//数据编号
	list<struct data_apk> list;
} APK_LIST;



typedef struct connect_list // 记录链接时间,用于心跳检测
{
	uint32 status;	//状态,用于判断是否已通过验证操作
	time_t time;
} CONNECT_LIST;



#endif














