#include "commponent/socket.h"
#include "commponent/thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SERVER_PORT 8000
#include <map>
#include <list>
#include <iostream>
using namespace std;

typedef struct server_msg_list1
{
	list<char*> send_msg_list;
} server_msg_list_t1;

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

	while (1)
	{
		char buf[10] = {0x00};
		gets(buf);
		printf("%s\n", buf);
		client_send_msg(buf, strlen(buf));
	}
	sleep(5);
	thread_pool_destroy(&pool);
	tcp_client_end();

	return 0;
}



