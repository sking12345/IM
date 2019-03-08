#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"


void *work(void *arg)
{
	printf("%s\n", "ddd");
	return NULL;
}

int main()
{
	struct server_base* pserver = tcp_server_init(8888, 10);
	tcp_server_start(pserver, 10);
	tcp_server_end(&pserver);
	//
	// struct thread_pool * pool = thread_pool_init(10, 10);
	// thread_add_job(pool, work, (void*)"dd");
	// thread_add_job(pool, work, (void*)"dd");
	// thread_add_job(pool, work, (void*)"dd");
	// thread_add_job(pool, work, (void*)"dd");

	// thread_add_job(pool, work, (void*)"dd");
	// printf("%s\n", "end");
	// sleep(1);
	// thread_pool_destroy(&pool);
	return 0;
}