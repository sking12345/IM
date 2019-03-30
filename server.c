#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"



void * work1(int cfd, void * read_buf, struct server_base *sbase)
{

	// printf("%s\n", (char*)read_buf);
	// char *buf = "ddd";
	// if (tcp_server_closed(sbase, cfd) <= 0)
	// {
	// 	tcp_send(cfd, (void*)buf, strlen(buf) + 1);
	// }
	const char * file_name = "/Users/mac/Desktop/xxx1.yuv";
	printf("xxxx_len::%ld\n", strlen((char*)read_buf));
	int yuv_fd = open(file_name, O_RDWR | O_CREAT | O_APPEND, 0777);
	write(yuv_fd, read_buf, strlen(read_buf));
	close(yuv_fd);
	return NULL;
}



int main() {

	struct thread_pool * pool = thread_pool_init(1, 10);

	struct server_base* sbase = tcp_server_init(8888, 10, 100, NULL);
	tcp_server_start(sbase, pool, NULL, NULL, work1);

	return 0;
}






