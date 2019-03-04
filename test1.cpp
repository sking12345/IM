
#include "component/types.h"
#include "component/client_socket.h"
#include <stdio.h>
#include <stdlib.h>
#define SERVER_PORT 8000


void* work(void* arg)
{

	struct send_buf *recv_buf = (struct send_buf*)arg;
	printf("%s\n", recv_buf->buf);
	return NULL;
}




int main()
{
	const char *ip = "127.0.0.1";
	int sfd = tcp_client_start(ip, SERVER_PORT);
	struct socket_info * sinfo = set_call_function(sfd, work, NULL);
	while (1)
	{
		char buf[10] = {0x00};
		gets(buf);
		send_msg(sfd, buf, strlen(buf));
	}
	sleep(5);
	tcp_client_end(&sinfo);
	return 0;
}