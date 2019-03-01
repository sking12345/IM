#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H
#include <map>
#include <list>
#include "types.h"
#include <iostream>
using namespace std;

class message_base
{

public:
	message_base();
	~message_base();
	virtual void new_message(int fd, void *buf); //新的数据处理
	virtual void socket_abnormal(int fd);	//socket 异常
};
#endif