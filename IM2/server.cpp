
#include "component/socket.h"
#include <iostream>
using namespace std;
#define PORT 8000

int main() {

	const char *ip = "127.0.0.1";

	tcp_server_start(PORT, 10);

	return 0;
}