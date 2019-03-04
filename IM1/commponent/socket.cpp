#include "socket.h"


struct tcp_server* tcp_server = NULL;

int tcp_server_start(int port, int listen_num, message_base*msg) {
	if (msg == NULL) {
		printf("%s\n", "error:tcp_server_start message_base don't NULL");
		return -1;
	}
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
	if (tcp_server == NULL) {
		tcp_server = (struct tcp_server*)malloc(sizeof(struct tcp_server));
	}
	tcp_server->msg_obj = msg;
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
void tcp_server_read(int fd, short events, void *arg) {

	struct data_apk apk;
	struct event *ev = (struct event*)arg;
	int len = read(fd, &apk, sizeof(struct data_apk));
	if ( len <= 0 ) {
		tcp_server->msg_obj->abnormal(fd);
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {

		if (apk.status == 0x01) {	//数据发送完毕
			map<int, struct apk_list>::iterator iter = tcp_server->apk_list_map.find(fd);
			if (iter == tcp_server->apk_list_map.end()) {
				tcp_server->msg_obj->new_msg(fd, (char*)&apk.buf, apk.size + 1);
			} else {
				struct apk_list *apk_list = &iter->second;
				list<struct data_apk>::iterator iter_list;
				char *buf = (char*)malloc(apk.size + 1);
				memset(buf, 0x00, apk.size + 1);
				for (iter_list = apk_list->list.begin(); iter_list != apk_list->list.end(); iter_list++) {
					memcpy(buf + iter_list->number * APK_SIZE, iter_list->buf, APK_SIZE);
				}
				tcp_server->msg_obj->new_msg(fd, buf, apk.size + 1);
				free(buf);
				buf = NULL;
			}
		} else {
			map<int, struct apk_list>::iterator iter = tcp_server->apk_list_map.find(fd);
			if (iter == tcp_server->apk_list_map.end()) {
				struct apk_list apk_list;
				apk_list.socket_fd = fd;
				apk_list.time = time(NULL);
				apk_list.list.push_back(apk);
				tcp_server->apk_list_map.insert(pair<int, struct apk_list>(fd, apk_list));
			} else {
				struct apk_list *apk_list = &iter->second;
				apk_list->list.push_back(apk);
			}
		}
	}
	return ;
}
void tcp_server_accept(int fd, short events, void* arg) {
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);

	tcp_server->msg_obj->new_accept(fd);
	struct event_base* base = (struct event_base*)arg;

	//仅仅是为了动态创建一个event结构体
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST,
	             tcp_server_read, (void*)ev);
}

void tcp_server_stop() {
	if (tcp_server != NULL) {
		event_base_free(tcp_server->evt_base);
		event_free(tcp_server->ev_listen);
		free(tcp_server);
		tcp_server = NULL;
	}

}









