#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"


void *read_call(void *arg) {
	return NULL;
}

int main() {

	struct client_base * cbase = tcp_client_init("127.0.0.1", 8888);
	if (cbase == NULL)
	{
		printf("%s\n", "err");
		return 0;
	}
	int fd = tcp_client_start(cbase, NULL, NULL);
	while (1)
	{
		for (int i = 0; i < 100; ++i)
		{
			const char *buf  = "ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
			if (tcp_client_closed(cbase) <= 0)
			{
				tcp_send(fd, (void*)buf, strlen(buf) + 1);
			}

			// sleep(1);
		}
		sleep(3);
	}

	sleep(10);
	return 0;
}