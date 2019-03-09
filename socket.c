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
	sread.data_size = len;
	sread.ev =  paccept->ev;
	sread.data_buf = msg;
	sread.arg = paccept->pserver->arg;
#if SERVER_READ_TYPE == 0x00
	thread_add_job(paccept->pserver->thread_pool, paccept->pserver->new_data, (void*)&sread);
#endif
#if SERVER_READ_TYPE == 0x01
	paccept->pserver->new_data(sread);
#endif

}
//接受数据的连接fd
int get_server_read_fd(void *sread)
{
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->fd;
}
//接受到的数据大小
int get_server_read_size(void *sread)
{
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->data_size;
}
//接受到的数据
char* get_server_read_buf(void *sread)
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
	pserver->read_call = read_call;
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
struct client_base * tcp_client_init(const char *ipstr, int port)
{
	int sockfd;

	struct sockaddr_in serveraddr;
	//1.创建一个socket
	int confd = socket(AF_INET, SOCK_STREAM, 0);
	if (confd < 0)
	{
		log_print("socket fail");
		return NULL;
	}
	//2.初始化服务器地址
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//"192.168.6.254"
	inet_pton(AF_INET, ipstr, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port  = htons(port);
	//3.链接服务器
	if (connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		log_print("connect fail");
		return NULL;
	}
	struct client_base * base = (struct client_base *)malloc(sizeof(struct client_base));
	base->ip = (char*)ipstr;
	base->port = port;
	base->fd = confd;
	return base;
}
void set_client_thread_pool(struct client_base* cbase, struct thread_pool *pool, void*(*read_call)(void *cread))
{
	cbase->thread_pool = pool;
	cbase->read_call = read_call;
}

void *client_read_thread(void *arg)
{
	struct client_base* cbase = (struct client_base*)arg;
	int fd = cbase->fd;
	while (1)
	{
		char msg[TCP_APK_SIZE + 1] = {0x00};
		int len = read(fd, msg, sizeof(msg));
		if ( len <= 0 ) {
			printf("close %d\n", fd);
			close(fd);
			return NULL;
		}
		printf("%s\n", msg);
		struct client_read cread;
		cread.fd = fd;
		cread.data_size = len;
		cread.data_buf = msg;
#if READ_CALL_TYEP == 0x00	//调用线程池去调用设置的回调函数
		thread_add_job(cbase->thread_pool, cbase->read_call, (void*)&cread);
#endif
#if READ_CALL_TYEP == 0x01	//直接调用设置的回调的函数
		cbase->read_call(&cread);
#endif
	}
	return NULL;
}

int tcp_client_start(struct client_base* cbase)
{
#if READ_CALL_TYEP == 0x00	//调用线程池去调用设置的回调函数
	if (cbase->thread_pool == NULL)
	{
		log_print("plase set thread poll");
		return -1;
	}
#endif
	pthread_t read_thread;
	pthread_create(&read_thread, NULL, client_read_thread, (void*)cbase);
	return 1;
}

#if TCP_QUEEU_TYPE == 0x01
int tcp_client_send(struct client_base*cbase, char *buf, int size, int priority)	//发送数据函数
{
	int _size = sizeof(struct send_queue) + size;
	struct send_queue * squeue = (struct send_queue*)malloc(_size);
	memset(squeue, 0x00, _size);
	squeue->size = size;
	squeue->next = NULL;
	memcpy(squeue->buf, buf, size);
	if (priority == 0x01)	//优先发送
	{

	} else {

	}
	printf("%s\n", squeue->buf);
	// send_data(cbase->fd, buf, size);
	// if (send(fd, buf, buf_size, 0) < 0)
	// {
	// 	log_print("error:send fail");
	// 	return -1;
	// }
	return 1;
}
#else
int tcp_client_send(struct client_base*cbase, char *buf, int size)
{
	if (send(cbase->fd, buf, size, 0) < 0)
	{
		log_print("error:send fail");
		return -1;
	}
	return 1;
}
#endif

int get_client_fd(void *cread)
{
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->fd;
}
char * get_client_read_buf(void *cread)
{
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->data_buf;
}
int get_client_read_size(void *cread)
{
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->data_size;
}

void tcp_client_end(struct client_base **cbase)
{
	if (*cbase == NULL)
	{
		return;
	}
	if ((*cbase)->fd > 0)
	{
		close((*cbase)->fd);
	}
	free(*cbase);
	*cbase = NULL;
}

#endif














