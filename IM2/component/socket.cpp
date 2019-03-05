#include "socket.h"

struct tcp_server tcp_server;
struct tcp_client tcp_client;
void tcp_server_read(int fd, short events, void *arg) {
	struct apk apk;
	struct event *ev = (struct event*)arg;
	int len = read(fd, &apk, sizeof(struct apk));
	if ( len <= 0 ) {
		printf("%s\n", "colse");
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {
		map<int, struct apk_list*>::iterator iter = tcp_server.apk_list_map.find(fd);
		if (apk.status == 0x01) {
			if (iter == tcp_server.apk_list_map.end()) {
				printf("%s\n", "once");
			} else {
				struct apk_list *apk_lsit = iter->second;
			}
		} else if (apk.status == 0x00) {
			size_t apk_size = sizeof(struct apk);
			struct apk *papk = (struct apk*)malloc(apk_size);
			memset(papk, 0x00, apk_size);
			memcpy(papk, &apk, apk_size);
			struct apk_list *apk_lsit = (struct apk_list *)malloc(sizeof(struct apk_list));
			apk_lsit->apk = papk;
			apk_lsit->next = NULL;
			apk_lsit->tail = apk_lsit;
			if (iter == tcp_server.apk_list_map.end()) {
				printf("%s\n", "dd");
				tcp_server.apk_list_map.insert(pair<int, struct apk_list*>(fd, apk_lsit));
			} else {
				struct apk_list *apk_lsit = iter->second;
				apk_lsit->tail->next = apk_lsit;
				apk_lsit->tail = apk_lsit;
				printf("%s\n", "dd1");
			}
		}

		printf("%s\n", "new msg");
	}
}

void tcp_server_accept(int fd, short events, void* arg) {
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);
	//tcp_server.msg_obj->new_accept(fd);
	printf("%s\n", "new accept" );
	struct event_base* base = (struct event_base*)arg;
	//仅仅是为了动态创建一个event结构体
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST,
	             tcp_server_read, (void*)ev);

	event_add(ev, NULL);

}


int tcp_server_init(int port, int listen_num) {
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
	return listener;

error:
	errno_save = errno;
	evutil_closesocket(listener);
	errno = errno_save;
	return -1;
}
int tcp_server_start(int port, int listen_num) {
	int listener = tcp_server_init(port, listen_num);
	tcp_server.evt_base = event_base_new();
	tcp_server.ev_listen = event_new(tcp_server.evt_base, listener, EV_READ | EV_PERSIST, tcp_server_accept, (void*)tcp_server.evt_base);
	event_add(tcp_server.ev_listen, NULL);
	event_base_dispatch(tcp_server.evt_base);
	return true;
}

int tcp_server_end() {
	if (tcp_server.evt_base != NULL) {
		event_base_free(tcp_server.evt_base);
	}
	if (tcp_server.ev_listen != NULL) {
		event_free(tcp_server.ev_listen);
	}
	return true;
}

int tcp_client_init(const char*ip, int port) {
	struct sockaddr_in serveraddr;
	int confd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port  = htons(port);
	//3.链接服务器
	int ret = connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret < 0) {
		printf("erro:%s connect fail\n", (char*)ip);
		return false;
	}
	tcp_client.socket_fd = confd;
	tcp_client.last_time = time(NULL);
	return confd;
}
int tcp_client_start(const char*ip, int port) {
	int confd = tcp_client_init(ip, port);
	return confd;
}
int tcp_client_end() {
	close(tcp_client.socket_fd);
	return true;
}













