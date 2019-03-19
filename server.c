#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"



void *work(void *arg)
{
	char * buf = (char*)arg;
	// printf("work_buf: %s\n",  buf);
	return NULL;
}

void test(struct thread_pool * pool)
{
	char *str1 = "dddd11ooosodfosdfoaosfodsofdofdddd11ooosodfosdfoaosfodsofdofdddd11dddd11ooosodfosdfoaosfdddd11ooosodfosdfoaosfodsofdofdddd11ooosodfosdfoaosfodsofdofdddd11dddd11ooosodfosdfoaosf";

	for (int i = 0; i < 1000000; ++i)
	{

		char *buf1 = (char*)malloc(strlen(str1) + 1);
		// printf("%ld\n", strlen(str1));

		if (buf1 != NULL)
		{
			memset(buf1, 0x00, strlen(str1) + 1);
			memcpy(buf1, str1, strlen(str1));
			thread_add_job(pool, work, (void*)buf1, strlen(str1) + 1, -1);
			free(buf1);
			buf1 = NULL;
		}

	}
}

int main() {

	// struct thread_pool * pool = thread_pool_init(10, 10);

	// struct server_base* sbase = tcp_server_init(8888, 10, 100, NULL);
	// tcp_server_start(sbase, pool, NULL, NULL, work);

	// char *str = "dddd";
	// char *buf = (char*)malloc(strlen(str));
	// memcpy(buf, str, strlen(str));
	// //
	struct thread_pool * pool = thread_pool_init(3, 10);
	// thread_add_job(pool, work, (void**)&buf, -1, NULL);

	sleep(5);
	int num = 0;
	int count = 150;
	while (count--)
	{
		test(pool);
		num++;
		printf("%s\n", "new start" );
		printf("num-------:%d\n", num);
		sleep(15);
	}



// // thread_add_job(pool, work, (void*)"dd");
// // thread_add_job(pool, work, (void*)"dd");

// // thread_add_job(pool, work, (void*)"dd");
// printf("%s\n", "end");
	printf("%s\n", "end---------------------------------" );

	printf("%s\n", "start free pool");
// printf("buf_p:%p\n", buf);
// printf("buf_p1:%p\n", buf1);
	thread_pool_destroy(&pool);
	sleep(10);
	return 0;
}






