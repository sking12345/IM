#include "component/event_socket.h"
#include <iostream>
using namespace std;

#define PORT 8000
#define LISTEN_NUM 10

int main()
{

	tcp_server(PORT, LISTEN_NUM);

}

