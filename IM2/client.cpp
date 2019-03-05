
#include "component/socket.h"
#include <iostream>
using namespace std;

#define PORT 8000

int main() {

	const char *ip = "127.0.0.1";

	tcp_client_start(ip, PORT);
	sleep(2);
	tcp_client_end();
	return 0;
}