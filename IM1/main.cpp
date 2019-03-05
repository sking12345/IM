

#include "commponent/socket.h"
#include "commponent/message_base.h"
#include "commponent/thread_pool.h"
#include <iostream>
using namespace std;

#define PORT 8000
#define LISTEN_NUM 10


class msg: public message_base {

public:
	void new_accept(int fd)
	{
		printf("%s\n", "news xxx fd");

	}
	void new_msg(int fd, char *buf, int size)
	{
		printf("%s--%s\n", "news msg fd:", buf);
	}
	void abnormal(int fd)
	{
		printf("%s\n", "abnormal msg fd");
	}
};

void* work(void* arg)
{
	printf("%s\n", (char*)arg);
	sleep(1);
	return NULL;
}


int main() {

	tcp_server_start(PORT, LISTEN_NUM, new msg());
	tcp_server_end();
	//
	// struct thread_pool* pool = thread_pool_init(10, 100);
	// //set_destroy_stop(pool);
	// for (int i = 0; i < 100; ++i)
	// {
	// 	//thread_pool_add_job(pool, work, (void*)"xxx", 1);
	// }


	// printf("%s\n", "ddd");
	sleep(10);
	return 0;
}


















