#include "component/event_socket.h"
#include "component/message_base.h"
#include "component/thread_pool.h"
#include <iostream>

using namespace std;

#define PORT 8000
#define LISTEN_NUM 10


class msg: public message_base
{
public:
	void new_message(int fd, void *buf)
	{
		printf("%s\n", "ddd");
		const char * str = "ddd";
		socket_send(fd, str, 3);
	}
	void socket_abnormal(int fd)
	{

	}
};

void* work(void* arg)
{
	char *p = (char*) arg;
	printf("threadpool callback fuction : %s.\n", p);
	sleep(1);
	return NULL;
}

int main()
{
	// message_base *base = new msg();
	// tcp_server_start(PORT, LISTEN_NUM, base);
	//
	struct thread_pool* pool = thread_pool_init(2, 10);
	thread_pool_add_job(pool, work, (void*)"1");
	thread_pool_add_job(pool, work, (void*)"2");
	thread_pool_add_job(pool, work, (void*)"3");
	thread_pool_add_job(pool, work, (void*)"4");
	thread_pool_add_job(pool, work, (void*)"5");
	thread_pool_add_job(pool, work, (void*)"6");
	// threadpool_add_job(pool, work, "7");
	sleep(5);
	thread_pool_destroy(pool);
	return 0;
}









