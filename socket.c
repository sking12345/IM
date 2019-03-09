#include "socket.h"

#if COMPILE_TYPE == 0x00


struct server_accept * server_accept_new() {
	struct server_accept* paccept = (struct server_accept*)malloc(sizeof(struct server_accept));
	return paccept;
}
void server_accept_free(struct server_accept** paccept) {
	free(*paccept);
	*paccept = NULL;
}
void accept_cb(int fd, short events, void* arg) {
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);
	printf("accept a client %d\n", sockfd);
	struct server_base *pserver = (struct server_base *)arg;
	struct server_accept *paccept = server_accept_new();
	if (paccept == NULL) {
		tcp_server_end(&pserver);
		return;
	}
	paccept->pserver = pserver;
	//仅仅是为了动态创建一个event结构体
	paccept->ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数
	event_assign(paccept->ev, pserver->base, sockfd, EV_READ | EV_PERSIST,
	             socket_read_cb, (void*)paccept);

	event_add(paccept->ev, NULL);
}


void socket_read_cb(int fd, short events, void *arg) {
	struct server_accept *paccept  = (struct server_accept*)arg;
	char msg[TCP_APK_SIZE + 1] = {0x00};
	int len = read(fd, msg, sizeof(msg));
	if ( len <= 0 ) {
		printf("close %d\n", fd);
		event_free(paccept->ev);
		close(fd);
		return ;
	}
	struct server_read sread;
	sread.fd = fd;
	sread.ev =  paccept->ev;
	sread.data_buf = msg;
	sread.arg = NULL;
#if SERVER_READ_TYPE == 0x00
	thread_add_job(paccept->pserver->thread_pool, paccept->pserver->new_data, (void*)&sread);
#endif

}

int get_server_read_fd(void *sread)
{
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->fd;
}
char* get_server_read_data(void *sread)
{
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->data_buf;
}



typedef struct sockaddr SA;
struct server_base * tcp_server_init(int port, int listen_num) {
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
	pserver->arg = NULL;
	//跨平台统一接口，将套接字设置为非阻塞状态
	evutil_make_socket_nonblocking(listener);
	pserver->base = event_base_new();
	pserver->ev_listen = event_new(pserver->base, listener, EV_READ | EV_PERSIST, accept_cb, pserver);
	event_add(pserver->ev_listen, NULL);
	return pserver;
}

void set_server_arg(struct server_base*pserver, void*arg) {
	pserver->arg = arg;
}

void set_server(struct server_base* pserver, struct thread_pool* pool, void*arg) {
	pserver->arg = arg;
	pserver->thread_pool = pool;
}

void set_server_call(struct server_base* pserver, void* (*new_accept)(int cfd),
                     void* (*new_data)(void *sread),
                     void* (*abnormal)(int cfd))
{
	pserver->new_accept = new_accept;
	pserver->new_data = new_data;
	pserver->abnormal = abnormal;
}

int tcp_server_start(struct server_base * pserver, int thread_num) {
#if SERVER_READ_TYPE == 0x00
	if (pserver->thread_pool == NULL)
	{
		log_debug("tcp server start fail,plase set server thread poll", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
#endif
	if (pserver->new_data == NULL)
	{
		log_debug("tcp server start fail,plase set server new data call function", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	event_base_dispatch(pserver->base);
	return 1;
}

void tcp_server_end(struct server_base **pserver) {
	event_free((*pserver)->ev_listen);
	event_base_free((*pserver)->base);
	free(*pserver);
	*pserver = NULL;
}
#else

#endif














