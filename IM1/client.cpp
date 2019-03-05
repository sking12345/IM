#include "commponent/socket.h"
#include "commponent/thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SERVER_PORT 8000




void* work(void* arg)
{

	printf("%s\n", (char*)arg);
	return NULL;
}


int main()
{

	const char *ip = "127.0.0.1";
	struct thread_pool* pool = thread_pool_init(1, 10);
	int sfd = tcp_client_start(ip, SERVER_PORT);
	set_client_call_function(sfd, work, NULL);
	set_client_thread_pool(&pool);
	sleep(1);
	thread_pool_destroy(&pool);
	tcp_client_end();
	// //
	// struct thread_pool* pool = thread_pool_init(10, 100);
	// // set_destroy_stop(pool);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);
	// thread_pool_add_job(pool, work, (void*)"xxxd", 4);

	// sleep(5);
	// thread_pool_destroy(&pool);
	// printf("%p\n", pool);
	// sleep(5);
	return 0;
}



