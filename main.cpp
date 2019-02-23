#include "component/event_socket.h"
#include <iostream>
using namespace std;

#define PORT 8000
#define LISTEN_NUM 10

int main()
{
	int listener = tcp_server_init(PORT, LISTEN_NUM);
}

