#include "socket.h"


//https://www.cnblogs.com/wainiwann/p/7096245.html



void accept_cb(int fd, short events, void* arg) {

	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	struct server_base *pserver = (struct server_base *)arg;
	if (fd > pserver->max_connect) {
		log_print("Out of connection");
		close(fd);
		return;
	}
	printf("%s\n", "new connect");
	pserver->connect_num ++;

	evutil_make_socket_nonblocking(sockfd);

	// char sevnts[sizeof(struct accepts_event)] = {0x00};


	printf("fd1:%d\n", fd);
	/**
	 * 做一个列表，根据fd 查找event_new 的地址
	 */
	struct event * evt = event_new(NULL, -1, 0, NULL, NULL);
	struct accepts_event accept_evt;
	accept_evt.cfd =  sockfd;
	accept_evt.evt = evt;
	accept_evt.recv_buf = NULL;
	memcpy(pserver->conencts_info + sockfd * sizeof(struct accepts_event), &accept_evt, sizeof(struct accepts_event));
	//将动态创建的结构体作为event的回调参数
	event_assign(evt, pserver->base, sockfd, EV_READ | EV_PERSIST,
	             socket_read_cb, (void*)pserver);

	event_add(evt, NULL);
}

void socket_read_cb(int fd, short events, void *arg) {

	struct server_base *sbae = (struct server_base *)arg;
	char sevnts[sizeof(struct accepts_event)] = {0x00};
	memcpy(&sevnts, sbae->conencts_info + fd * sizeof(struct accepts_event), sizeof(struct accepts_event));
	struct accepts_event * accept_evt = (struct accepts_event *)sevnts;

	char msg[4096];
	// struct event *ev = (struct event*)arg;
	int len = read(fd, msg, sizeof(msg) - 1);
	if ( len <= 0 ) {
		event_free(accept_evt->evt);
		close(fd);
		return ;
	}
	msg[len] = '\0';
	printf("fd:%d,%s\n", accept_evt->cfd, msg);

}


struct server_base* tcp_server_init(int port, int listen_num, int max_connect, void *arg) {
	int errno_save;
	evutil_socket_t listener;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if ( listener == -1 )
		return NULL;

	//允许多次绑定同一个地址。要用在socket和bind之间
	evutil_make_listen_socket_reuseable(listener);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);
	if (bind(listener, (SA*)&sin, sizeof(sin)) < 0 ) {
		errno_save = errno;
		evutil_closesocket(listener);
		errno = errno_save;
		return NULL;
	}
	if ( listen(listener, listen_num) < 0) {
		errno_save = errno;
		evutil_closesocket(listener);
		errno = errno_save;
		return NULL;
	}
	struct server_base *pserver = (struct server_base *)malloc(sizeof(struct server_base ));
	pserver->arg = arg;
	pserver->max_connect = max_connect;
	pserver->conencts_info = (char*)malloc(sizeof(struct accepts_event) * max_connect);
	memset(pserver->conencts_info, 0x00, sizeof(struct accepts_event) * max_connect);
	//跨平台统一接口，将套接字设置为非阻塞状态
	evutil_make_socket_nonblocking(listener);
	pserver->base = event_base_new();
	pserver->ev_listen = event_new(pserver->base, listener, EV_READ | EV_PERSIST, accept_cb, pserver);
	event_add(pserver->ev_listen, NULL);
	return pserver;
}

int tcp_server_start(struct server_base*sbase, struct thread_pool *tpool, void* (*new_accept)(int cfd), void* (*abnormal)(int cfd), void* (*read_call)(void **recv_buf)) {
	if (sbase == NULL) {
		log_print("struct server_base is null");
		return -1;
	}
	sbase->thread_pool = tpool;
	sbase->abnormal = abnormal;
	sbase->new_accept = new_accept;
	sbase->read_call = read_call;
	event_base_dispatch(sbase->base);
	return 1;
}









