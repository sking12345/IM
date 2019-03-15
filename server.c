#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"





int main() {


	struct server_base* sbase = tcp_server_init(8888, 10, 100, NULL);
	tcp_server_start(sbase, NULL, NULL, NULL, NULL);

	// return 0;
	// int fd = 1;
	// struct thread_pool * pool = thread_pool_init(10, 10);
	// thread_add_job(pool, work, (void*)&fd);
	// // thread_add_job(pool, work, (void*)"dd");
	// // thread_add_job(pool, work, (void*)"dd");
	// // thread_add_job(pool, work, (void*)"dd");

	// // thread_add_job(pool, work, (void*)"dd");
	// printf("%s\n", "end");
	// sleep(1);
	// thread_pool_destroy(&pool);
	return 0;
}