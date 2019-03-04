#include "component/event_socket.h"
#include "component/message_base.h"
#include "component/thread_pool.h"
#include <iostream>

using namespace std;

#define PORT 8000
#define LISTEN_NUM 10



void* work(void* arg)
{
	struct test_apk *apk = (struct test_apk*)arg;

	printf("threadpool %d callback fuction : %s\n", apk->test, apk->buf);

	socket_send( apk->test, apk->buf , strlen(apk->buf));
	sleep(1);
	return NULL;
}


class msg: public message_base
{
private:
	struct thread_pool* pool;
public:
	msg()
	{
		pool = thread_pool_init(4, 10);
	}
	void new_message(int fd, void *buf)
	{
		struct test_apk *apk = (struct test_apk*)buf;
		apk->test = fd;
		thread_pool_add_job(pool, work, (void*)apk);
	}
	void socket_abnormal(int fd)
	{

	}
};



int main()
{
	message_base *base = new msg();
	tcp_server_start(PORT, LISTEN_NUM, base);
	//
	// struct thread_pool* pool = thread_pool_init(2, 10);
	// set_destroy_stop(pool);
	// thread_pool_add_job(pool, work, (void*)"1");
	// thread_pool_add_job(pool, work, (void*)"2");
	// thread_pool_add_job(pool, work, (void*)"3");
	// thread_pool_add_job(pool, work, (void*)"4");
	// thread_pool_add_job(pool, work, (void*)"5");
	// thread_pool_add_job(pool, work, (void*)"6");
	// // threadpool_add_job(pool, work, "7");
	// sleep(5);
	// thread_pool_destroy(&pool);
	return 0;
}









