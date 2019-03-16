#include "socket.h"


//https://www.cnblogs.com/wainiwann/p/7096245.html


int tcp_send(int fd, void *buf, int data_size)
{
	printf("tcp_send::%s\n", buf);
	struct apk_buf apk = {0x00};
	int send_size = sizeof(struct apk_buf);
	apk.size = data_size;
	int residue = data_size % TCP_APK_SIZE;
	if (residue > 0) {
		int count = (data_size - residue) / TCP_APK_SIZE;
		for (int i = 0; i < count; ++i) {
			apk.number = i;
			apk.status = APK_NOT_END;
			memset(apk.buf, 0x00, TCP_APK_SIZE);
			memcpy(apk.buf, buf + i * TCP_APK_SIZE, TCP_APK_SIZE);
			int status = send(fd, &apk, send_size, 0);
			if (status < 0) {
				log_print("send apk err2");
				return status;
			}
		}
		apk.status = APK_END;
		apk.number = count;
		memset(apk.buf, 0x00, TCP_APK_SIZE);
		memcpy(apk.buf, buf + count * TCP_APK_SIZE, residue);
		int status = send(fd, &apk, send_size, 0);
		if (status < 0) {
			log_print("send apk err1");
			return status;
		}
	} else {
		int count = (data_size / TCP_APK_SIZE);
		for (int i = 0; i < count; ++i) {
			apk.number = i;
			if (i + 1 < count) {
				apk.status = APK_NOT_END;
			} else {
				apk.status = APK_END;
			}
			memset(apk.buf, 0x00, TCP_APK_SIZE);
			memcpy(apk.buf, buf + i * TCP_APK_SIZE, TCP_APK_SIZE);
			int status = send(fd, &apk, send_size, 0);
			if (status < 0) {
				log_print("send apk erro3");
				return status;
			}
		}
	}
	return 0;
}


#if COMPILE_TYPE == 0x00
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
	struct event * evt = event_new(NULL, -1, 0, NULL, NULL);
	struct accepts_event accept_evt;
	accept_evt.cfd =  sockfd;
	accept_evt.evt = evt;
	accept_evt.recv_buf = NULL;
	accept_evt.status = 0x00;
	memcpy(pserver->conencts_info + sockfd * sizeof(struct accepts_event), &accept_evt, sizeof(struct accepts_event));
	//将动态创建的结构体作为event的回调参数
	event_assign(evt, pserver->base, sockfd, EV_READ | EV_PERSIST,
	             socket_read_cb, (void*)pserver);

	event_add(evt, NULL);
}

void test_free(char ** buf)
{
	free(*buf);
	*buf = NULL;
}

void socket_read_cb(int fd, short events, void *arg) {
	struct server_base *sbaes = (struct server_base *)arg;
	char sevnts[sizeof(struct accepts_event)] = {0x00};
	memcpy(&sevnts, sbaes->conencts_info + fd * sizeof(struct accepts_event), sizeof(struct accepts_event));
	struct accepts_event * accept_evt = (struct accepts_event *)sevnts;
	struct apk_buf recv_apk = {0x00};
	int len = read(fd, &recv_apk, sizeof(struct apk_buf));
	if ( len <= 0 ) {

		printf("%s\n", "colse");
		event_free(accept_evt->evt);
		accept_evt->evt = NULL;
		accept_evt->status = 0x00;
		memcpy(sbaes->conencts_info + fd * sizeof(struct accepts_event), accept_evt, sizeof(struct accepts_event));
		close(fd);
		return ;
	}
	if (recv_apk.status == APK_CONFIRM)
	{
		return;
	}
	if (accept_evt->status == 0x00)
	{
		accept_evt->recv_buf = (char*)malloc(recv_apk.size);
		memset(accept_evt->recv_buf, 0x00, recv_apk.size);
		accept_evt->status = 0x01;
		printf("%s\n", "malloc");
	}
	memcpy(accept_evt->recv_buf + recv_apk.number * TCP_APK_SIZE, recv_apk.buf, TCP_APK_SIZE);
	if (recv_apk.status == APK_END)
	{
		accept_evt->status = 0x00;
		printf("%p--%s\n", accept_evt->recv_buf, accept_evt->recv_buf);
		// char *str = "dddd";
		char *buf = (char*)malloc(recv_apk.size + 1);
		if (buf != NULL)
		{
			memset(buf, '\0', recv_apk.size + 1);
			memcpy(buf, accept_evt->recv_buf, recv_apk.size);
			//thread_add_job(sbaes->thread_pool, sbaes->read_call, (void**)&buf, -1, NULL);
		} else {
			printf("%s\n", "malloc error");
		}
		free(accept_evt->recv_buf);
		accept_evt->recv_buf = NULL;
		memcpy(sbaes->conencts_info + fd * sizeof(struct accepts_event), accept_evt, sizeof(struct accepts_event));


	} else {
		memcpy(sbaes->conencts_info + fd * sizeof(struct accepts_event), accept_evt, sizeof(struct accepts_event));

	}

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

#else
struct client_base * tcp_client_init(const char *ipstr, int port)
{

	struct sockaddr_in serveraddr;
	//1.创建一个socket
	int confd = socket(AF_INET, SOCK_STREAM, 0);
	if (confd < 0) {
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
	if (connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
		log_print("connect fail");
		return NULL;
	}
	struct client_base * base = (struct client_base *)malloc(sizeof(struct client_base));
	base->ip = (char*)ipstr;
	base->port = port;
	base->sfd = confd;

	return base;
}

void *client_read_thread(void *arg)
{
	struct client_base* cbase = (struct client_base*)arg;
	int fd = cbase->sfd;
	while (1) {
		struct apk_buf recv_apk;
		int len = recv(fd, &recv_apk, sizeof(struct apk_buf), 0);
		if ( len <= 0 ) {
			close(fd);
			return NULL;
		}
		printf("%s\n", recv_apk.buf);
	}
}

int tcp_client_start(struct client_base *cbase, void* (*abnormal)(int cfd), void* (*read_call)(void **recv_buf))
{
	if (cbase == NULL)
	{
		log_print("struct client_base is NULL");
		return -1;
	}
	cbase->abnormal = abnormal;
	cbase->read_call = read_call;
	pthread_t read_thread;
	pthread_create(&read_thread, NULL, client_read_thread, (void*)cbase);
	return cbase->sfd;
}


#endif


















