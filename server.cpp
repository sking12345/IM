
#include "component/socket.h"
#include <iostream>
using namespace std;
#define PORT 8000



void * new_apk(int fd, struct apk*recv_apk)
{
	printf("%s\n", "xx");
	return NULL;
}

void * new_data(int fd, void *arg)
{
	return NULL;
}

void * abnormal(int fd)
{
	return NULL;
}



int main() {

	const char *ip = "127.0.0.1";
	tcp_server_start(PORT, 10, 20);

	return 0;
}