#ifndef MASSAGE_BASE_H
#define MASSAGE_BASE_H
#include <stdlib.h>
#include <stdio.h>

class message_base {
  public:
	message_base();
	virtual ~message_base();
	virtual void connect_success(int fd);	//连接成功
	virtual void new_msg(int fd, char *buf, int size);
	virtual void abnormal(int fd);
};
#endif