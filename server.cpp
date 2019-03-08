#include <stdio.h>
#include <stdlib.h>
#include "socket.h"

int main()
{
	struct server_t* pserver = tcp_server_init(8888, 10);
	tcp_server_start(pserver, 10);
	return 0;
}