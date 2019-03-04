/**
 * 逻辑处理基类
 */

#ifndef MASSAGE_BASE_H
#define MASSAGE_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include "type.h"

class message_base {

  public:
	message_base();
	~message_base();
	void new_accept(int fd);
	void new_msg(int fd, char *buf, int size);
	void abnormal(int fd);
	void set_thread_pool();
};


#endif