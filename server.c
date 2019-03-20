#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"
// #include "log.h"


void * work1(int cfd, void * read_buf, struct server_base *sbase)
{
	printf("%s\n", (char*)read_buf);
	char *buf = "ddd";
	if (tcp_server_closed(sbase, cfd) <= 0)
	{
		tcp_send(cfd, (void*)buf, strlen(buf) + 1);
	}

	return NULL;
}

// void *work(void *arg)
// {
// 	// struct accepts_event * accept = (struct accepts_event*)arg;;
// 	// printf("work_buf1: %s\n", accept->recv_buf);
// 	struct read_buf *rbuf = (struct read_buf*)arg;
// 	printf("rbuf->buf:%d\n", rbuf->buf);
// 	return NULL;
// }



int main() {

	struct thread_pool * pool = thread_pool_init(1, 10);

	struct server_base* sbase = tcp_server_init(8888, 10, 100, NULL);
	tcp_server_start(sbase, pool, NULL, NULL, work1);

	return 0;
}






