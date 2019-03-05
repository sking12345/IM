/**
 * 逻辑处理基类
 */

#ifndef MASSAGE_BASE_H
#define MASSAGE_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include "type.h"
#include <unistd.h>

class message_base {

public:
	message_base();
	virtual ~message_base();
	virtual void new_accept(int fd);
	virtual void new_msg(int fd, char *buf, int size);
	virtual void abnormal(int fd);
};


#endif