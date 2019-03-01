#include "component/event_socket.h"
#include "component/message_base.h"
#include <iostream>

using namespace std;

#define PORT 8000
#define LISTEN_NUM 10


class msg: public message_base
{
public:
	void new_message(int fd, void *buf)
	{
		printf("%s\n", "ddd");
	}
	void socket_abnormal(int fd)
	{

	}
};



int main()
{
	message_base *base = new msg();
	// base->new_message(1, NULL);

	tcp_server_start(PORT, LISTEN_NUM, base);
	return 0;
}

