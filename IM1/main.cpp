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
	struct thread_pool* pool = thread_pool_init(1, 10);
	thread_pool_add_job(pool, work, (void*)"1");
	// threadpool_add_job(pool, work, "2");
	// threadpool_add_job(pool, work, "3");
	// threadpool_add_job(pool, work, "4");
	// threadpool_add_job(pool, work, "5");
	// threadpool_add_job(pool, work, "6");
	// threadpool_add_job(pool, work, "7");
	sleep(5);
	thread_pool_destroy(pool);
	return 0;
}









