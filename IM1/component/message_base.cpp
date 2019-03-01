#include "message_base.h"

message_base::message_base()
{

}

message_base::~message_base()
{

}

void message_base::new_message(int fd, void *buf)
{
	printf("%s\n", "xxx");
}


void message_base::socket_abnormal(int fd)
{

}


