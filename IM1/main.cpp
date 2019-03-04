

#include "commponent/socket.h"
#include "commponent/message_base.h"
#include <iostream>
using namespace std;

#define PORT 8000
#define LISTEN_NUM 10


class msg: public message_base {

};


int main() {

	tcp_server_start(PORT, LISTEN_NUM, new msg());

	tcp_server_stop();

	printf("%s\n", "ddd");
	return 0;
}