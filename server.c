#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"



void *work(void **arg)
{
	char ** buf = (char**)arg;

	printf("work_buf:%s, %p\n", *buf, *buf);


	if (buf == NULL)
	{
		printf("%s\n", "buf error");
		return NULL;
	}
	// char ** buf = (char**)arg;

	// printf("%s\n", *buf);
	free(*buf);
	*buf = NULL;
	return NULL;
}


int main() {

	// struct thread_pool * pool = thread_pool_init(10, 10);

	// struct server_base* sbase = tcp_server_init(8888, 10, 100, NULL);
	// tcp_server_start(sbase, pool, NULL, NULL, work);

	// char *str = "dddd";
	// char *buf = (char*)malloc(strlen(str));
	// memcpy(buf, str, strlen(str));
	// //
	struct thread_pool * pool = thread_pool_init(10, 10);
	// thread_add_job(pool, work, (void**)&buf, -1, NULL);

	for (int i = 0; i < 10000; ++i)
	{
		char *str1 = "dddd11";
		char *buf1 = (char*)malloc(strlen(str1));
		printf("malloc1::%p\n", buf1);
		memcpy(buf1, str1, strlen(str1));
		sleep(1);
		thread_add_job(pool, work, (void**)&buf1, -1, NULL);
	}


	// // thread_add_job(pool, work, (void*)"dd");
	// // thread_add_job(pool, work, (void*)"dd");

	// // thread_add_job(pool, work, (void*)"dd");
	// printf("%s\n", "end");
	sleep(1);
	// printf("buf_p:%p\n", buf);
	// printf("buf_p1:%p\n", buf1);
	thread_pool_destroy(&pool);
	return 0;
}






