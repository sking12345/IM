
#include "component/socket.h"
#include <iostream>
using namespace std;

#define PORT 8000

int main() {

	const char *ip = "127.0.0.1";
	int sfd = tcp_client_init(ip, PORT);
	tcp_client_start(sfd);
	int opt = get_send_buf_size(sfd);

	printf("SO_KEEPALIVE Value: %d\n", opt);

	// sleep(3);
	const char *str = "ndd";
	printf("str len:%ld\n", strlen(str));
	int count = 8;
	int len = strlen(str);
	char *buf = (char*)malloc(len * count + 1);
	memset(buf, 0x00, len * count + 1);
	printf("%d\n", len * count + 1);
	for (int i = 0; i < count; ++i) {
		memcpy(buf + i * len, str, len);
	}
	for (int i = 0; i < 1; ++i) {
		printf("%d\n", i );
		int status = tcp_client_send(sfd, (char*)buf, len * count + 1);
		if (status < 0) {
			printf("%s\n", "error");
		}
	}

	free(buf);
	buf = NULL;
	// for (int i = 0; i < 1000; ++i)
	// {
	// 	// printf("%d\n", i );
	// 	tcp_client_send(sfd, (char*)str, strlen(str));
	// }
	// while (1)
	// {
	// 	char buf[100] = {0x00};
	// 	gets(buf);
	// 	if (strlen(buf) > 0)
	// 	{
	// 		tcp_client_send(sfd, buf, strlen(buf));
	// 	}
	// }
	sleep(2);
	//printf("%s\n", "end0");
	tcp_client_end(sfd);
	printf("%s\n", "end");
	// sleep(2);
	return 0;
}