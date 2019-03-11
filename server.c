#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"


void *work(void *arg) {
	int fd = *(int*)arg;

	printf("%s:%d\n", "ddd", fd);
	return NULL;
}


void * server_read_fun(void *sread) {
	int fd = get_server_read_fd(sread);
	char *str = get_server_read_buf(sread);
	printf("new msg:%d--%s\n", fd, str);
	//send_data(fd, "xxx", 3);
	return NULL;
}

int main() {


	struct thread_pool * server_pool = thread_pool_init(1, 10);
	struct server_base* pserver = tcp_server_init(8888, 10);
	set_server_call(pserver, NULL, server_read_fun, NULL);
	set_server(pserver, server_pool, NULL);
	tcp_server_start(pserver, 10);
	tcp_server_end(&pserver);


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