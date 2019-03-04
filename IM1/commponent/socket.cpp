#include "socket.h"


struct tcp_server* tcp_server = NULL;

int tcp_server_start(int port, int listen_num)
{
	int errno_save;
	evutil_socket_t listener;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if ( listener == -1 )
		return -1;

	//允许多次绑定同一个地址。要用在socket和bind之间
	evutil_make_listen_socket_reuseable(listener);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);

	if ( bind(listener, (struct sockaddr*)&sin, sizeof(struct sockaddr)) < 0 )
		goto error;

	if ( listen(listener, listen_num) < 0)
		goto error;

	//跨平台统一接口，将套接字设置为非阻塞状态
	evutil_make_socket_nonblocking(listener);
	if(tcp_server == NULL)
	{
		tcp_server = (struct tcp_server*)malloc(sizeof(struct tcp_server));
	}
	tcp_server->evt_base =  event_base_new();
	tcp_server->ev_listen =  event_new(tcp_server->evt_base, listener, EV_READ | EV_PERSIST, tcp_server_accept, (void*)tcp_server->evt_base);
	event_add(tcp_server->ev_listen, NULL);
	event_base_dispatch(tcp_server->evt_base);
	return listener;

error:
	errno_save = errno;
	evutil_closesocket(listener);
	errno = errno_save;
	return -1;

}
void tcp_server_read(int fd, short events, void *arg)
{

}
void tcp_server_accept(int fd, short events, void* arg)
{

}

void tcp_server_stop(struct tcp_server** tcp_server)
{

}