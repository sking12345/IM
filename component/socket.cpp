#include "socket.h"

struct tcp_server tcp_server;
struct tcp_client tcp_client;

/**
 * [send_err: 发送错误回调]
 * @param fd [description]
 */
void socket_send_err(int fd) {
	printf("%s:%d\n", "socket_send_err", fd);
}


int set_disable_nagle(int fd) {
	int on = 1;
	int nNetTimeout = 1000;
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&nNetTimeout, sizeof(int));
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&nNetTimeout, sizeof(int));
	return true;
}


int get_send_buf_size(int cfd) {
	int opt;
	socklen_t len1 = sizeof(int);
	if ((getsockopt(cfd, SOL_SOCKET, SO_RCVBUF, (char*)&opt, &len1)) == 0) {
		printf("SO_RCVBUF Value: %d\n", opt);
		return opt;
	}
	return -1;
}

int set_socket_buf_size(int socket_fd) {
	int nRcvBufferLen = 120 * 1024 * 1024  ;
	int nSndBufferLen = 64 * 1024 * 1024;
	int nLen          = sizeof(int);

	setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, (char*)&nSndBufferLen, nLen);
	setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, (char*)&nRcvBufferLen, nLen);
	return true;
}

//确认接受到消息
void confirm_recv(int cfd, int apk_status) {
	struct apk confirem_apk;
	confirem_apk.number = 0x00;
	confirem_apk.status = apk_status;
}
/**
 * [server_send_work 服务器端发送书数据]
 * @param arg [description]
 */
void* server_send_work(void* arg) {
	int confd = *(int*)arg;
	map<int, struct send_buf_list>::iterator iter = tcp_server.send_buf_map.find(confd);
	if (iter == tcp_server.send_buf_map.end()) {
		return NULL;
	}

	struct send_buf_list *slist = &iter->second;
	list<struct send_buf>::iterator iter_list = slist->send_list.begin();

	//发送完后就删除改
	struct apk send_apk;
	send_apk.size = iter_list->size;
	int residue = send_apk.size % APK_SIZE;
	int count = (send_apk.size - residue) / APK_SIZE;
	char *send_buf = iter_list->buf;
	int status;
	if (residue > 0) {
		send_apk.status = 0x00;
		for (int i = 0; i < count; ++i) {
			send_apk.number = i;
			memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
			memcpy(send_apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			status = write(confd, &send_apk, sizeof(send_apk));
			if (status <= 0) {
				socket_send_err(confd);
				return NULL;
			}
		}
		send_apk.number = count;

		send_apk.status = 0x01;
		memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
		memcpy(send_apk.buf, (char*)send_buf + count * APK_SIZE, residue);
		status = write(confd, &send_apk, sizeof(send_apk));
		if (status <= 0) {
			socket_send_err(confd);
			return NULL;
		}

	} else {
		for (int i = 0; i < count; ++i) {
			send_apk.number = i;
			memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
			if (i * APK_SIZE + APK_SIZE < send_apk.size) {
				memcpy(send_apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			} else {
				int opt = i * APK_SIZE;
				memcpy(send_apk.buf, (char*)send_buf + opt, send_apk.size - opt);
			}
			if (i == count - 1) {
				send_apk.status = 0x01;
			} else {
				send_apk.status = 0x00;
			}
			status = write(confd, &send_apk, sizeof(send_apk));
			if (status <= 0) {
				socket_send_err(confd);
				return NULL;
			}
		}
	}
	slist->send_list.erase(iter_list);
	return NULL;
}


void tcp_server_read(int fd, short events, void *arg) {
	struct apk apk_data;
	struct event *ev = (struct event*)arg;
	int len = read(fd, &apk_data, sizeof(struct apk));
	if ( len <= 0 ) {
		printf("%d %s\n", fd, "--------colse");
		map<int, struct recv_buf_list>::iterator iter = tcp_server.recv_buf_map.find(fd);
		if (iter != tcp_server.recv_buf_map.end())
		{
			struct recv_buf_list *recv_buf_list = &iter->second;
			recv_buf_list->recv_list.clear();
			tcp_server.recv_buf_map.erase(iter);
		}
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {
		get_send_buf_size(fd);
		map<int, struct recv_buf_list>::iterator iter = tcp_server.recv_buf_map.find(fd);
		if (apk_data.status !=  0x00 && apk_data.status !=  0x01 && apk_data.status !=  0x02)
		{
			printf("%s\n", "errorrrr");
			if (iter != tcp_server.recv_buf_map.end())
			{
				struct recv_buf_list *recv_buf_list = &iter->second;
				recv_buf_list->recv_list.clear();
				tcp_server.recv_buf_map.erase(iter);
			}
			close(event_get_fd(ev));
			event_free(ev);
			return ;
		}
		if (apk_data.status == 0x01 && apk_data.number == 0x00) {
			printf("%s\n", "new msg1" );
			return;
		}
		//map<int, struct recv_buf_list>::iterator iter = tcp_server.recv_buf_map.find(fd);
		if (iter == tcp_server.recv_buf_map.end())
		{
			struct recv_buf_list recv_buf_list;
			recv_buf_list.apk_count = 1;
			recv_buf_list.recv_list.push_back(apk_data);
			tcp_server.recv_buf_map.insert(pair<int, struct recv_buf_list>(fd, recv_buf_list));
		} else {
			struct recv_buf_list *recv_buf_list = &iter->second;
			if (recv_buf_list->apk_count >= APK_MAX_SIZE)
			{
				recv_buf_list->recv_list.clear();
				return;
			}
			if (apk_data.number == 0x00)
			{
				recv_buf_list->apk_count++;
			}
			recv_buf_list->recv_list.push_back(apk_data);
			++recv_buf_list->apk_count;
		}
		if (apk_data.status == 0x01)
		{

			map<int, struct recv_buf_list>::iterator iter1 = tcp_server.recv_buf_map.find(fd);
			struct recv_buf_list *recv_buf_list = &iter1->second;
			recv_buf_list->apk_count -= recv_buf_list->recv_list.size();
			recv_buf_list->recv_list.clear();
			--recv_buf_list->apk_count;
			printf("%s\n", apk_data.buf );
			return;
		}

	}
}

void tcp_server_accept(int fd, short events, void* arg) {
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);
	printf("%s\n", "new accept" );
	set_disable_nagle(fd);
	set_socket_buf_size(fd);
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
	set_socket_buf_size(listener);
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

	set_disable_nagle(listener);
	if (tcp_server.pool == NULL) {
		tcp_server.pool = thread_pool_init(10, 1000);
	}
	tcp_server.status = 0x00;
	tcp_server.evt_base = event_base_new();
	tcp_server.ev_listen = event_new(tcp_server.evt_base, listener, EV_READ | EV_PERSIST, tcp_server_accept, (void*)tcp_server.evt_base);
	event_add(tcp_server.ev_listen, NULL);
	event_base_dispatch(tcp_server.evt_base);
	return true;
}
/**
 * [tcp_server_send 服务端发送数据]
 * @param  fd   [description]
 * @param  buf  [description]
 * @param  size [description]
 * @return      [description]
 */
int tcp_server_send(int fd, char *buf, int size) {
	if (tcp_server.status == 0x01) {
		return -2;
	}
	if (size > get_send_buf_size(fd)) {
		return -3;
	}
	map<int, struct send_buf_list>::iterator iter = tcp_server.send_buf_map.find(fd);
	if (iter == tcp_server.send_buf_map.end()) {
		struct send_buf sbuf ;
		sbuf.size = size;
		sbuf.status = 0x00;
		sbuf.buf = (char*)malloc(size + 1);
		if (sbuf.buf == NULL) {
			printf("err:malloc\n");
			return -4;
		}
		memset(sbuf.buf, 0x00, size + 1);
		memcpy(sbuf.buf, buf, size);
		struct send_buf_list  slist;
		slist.send_list.push_back(sbuf);
		tcp_server.send_buf_map.insert(pair<int, struct send_buf_list>(fd, slist));
	} else {
		struct send_buf_list *slist = &iter->second;
		if (slist->send_list.size() > SEND_QUEUE_MAX) {
			return -1;
		}
		struct send_buf sbuf ;
		sbuf.size = size;
		sbuf.status = 0x00;
		sbuf.buf = (char*)malloc(size + 1);
		if (sbuf.buf == NULL) {
			printf("err:malloc\n");
			return -4;
		}
		memset(sbuf.buf, 0x00, size + 1);
		memcpy(sbuf.buf, buf, size);
		slist->send_list.push_back(sbuf);
	}
	thread_pool_add_job(tcp_server.pool, server_send_work, (void*)&fd, sizeof(int));
	return true;
}
/**
 * [tcp_server_end 服务器端结束]
 * @return [description]
 */
int tcp_server_end() {
	if (tcp_server.evt_base != NULL) {
		event_base_free(tcp_server.evt_base);
	}
	if (tcp_server.ev_listen != NULL) {
		event_free(tcp_server.ev_listen);
	}
	if (tcp_server.pool != NULL) {
		thread_pool_destroy(&tcp_server.pool);
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
	set_socket_buf_size(confd);
	//3.链接服务器
	int ret = connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret < 0) {
		printf("erro:%s connect fail\n", (char*)ip);
		return false;
	}
	// tcp_client.socket_fd = confd;
	tcp_client.last_time = time(NULL);
	return confd;
}

/**
 * 清空数据
 */

void tcp_client_clear(int fd) {
	map<int, struct send_buf_list>::iterator iter = tcp_client.send_buf_map.find(fd);
	if (iter != tcp_client.send_buf_map.end()) {
		struct send_buf_list *slist = &iter->second;
		list<struct send_buf>::iterator iter_list;

		for (iter_list = slist->send_list.begin(); iter_list != slist->send_list.end(); iter_list++) {
			if (iter_list->buf != NULL) {
				free(iter_list->buf);
				iter_list->buf = NULL;
			}

		}
		slist->send_list.clear();
		tcp_client.send_buf_map.erase(iter);
	}
	return;
}




void* client_send_work(void* arg) {
	int confd = *(int*)arg;
	map<int, struct send_buf_list>::iterator iter = tcp_client.send_buf_map.find(confd);
	if (iter == tcp_client.send_buf_map.end()) {
		return NULL;
	}

	struct send_buf_list *slist = &iter->second;
	list<struct send_buf>::iterator iter_list = slist->send_list.begin();
	//发送完后就删除改
	struct apk send_apk;
	send_apk.size = iter_list->size;
	int residue = send_apk.size % APK_SIZE;
	int count = (send_apk.size - residue) / APK_SIZE;
	char *send_buf = iter_list->buf;
	int status;
	if (residue > 0) {
		send_apk.status = 0x00;
		for (int i = 0; i < count; ++i) {
			send_apk.number = i;
			memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
			memcpy(send_apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			status = write(confd, &send_apk, sizeof(send_apk));
			if (status <= 0) {
				socket_send_err(confd);
				return NULL;
			}
		}
		send_apk.number = count;
		send_apk.status = 0x01;
		memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
		memcpy(send_apk.buf, (char*)send_buf + count * APK_SIZE, residue);
		status = write(confd, &send_apk, sizeof(send_apk));
		if (status <= 0) {
			socket_send_err(confd);
			return NULL;
		}
	} else {
		for (int i = 0; i < count; ++i) {
			send_apk.number = i;
			memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
			if (i * APK_SIZE + APK_SIZE < send_apk.size) {
				memcpy(send_apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			} else {
				int opt = i * APK_SIZE;
				memcpy(send_apk.buf, (char*)send_buf + opt, send_apk.size - opt);
			}
			if (i == count - 1) {
				send_apk.status = 0x01;
			} else {
				send_apk.status = 0x00;
			}
			status = write(confd, &send_apk, sizeof(send_apk));
			if (status <= 0) {
				socket_send_err(confd);
				return NULL;
			}
		}
	}
	slist->send_list.erase(iter_list);
	return NULL;
}

/**
 * [tcp_client_recv_thread 客户端接受数据线程]
 * @param arg [description]
 */
void* tcp_client_recv_thread(void* arg) {

	// int cfd = tcp_client.socket_fd;
	int cfd = *(int*)arg;
	printf("%d\n", cfd);
	char *buf = NULL;
	while (1) {
		struct apk recv_apk;
		int len = read(cfd, &recv_apk, sizeof(struct apk));
		if (len > 0) {
			if (buf == NULL) {
				buf = (char*)malloc(recv_apk.size + 1);
				if (buf == NULL) {
					printf("err:malloc\n");
					return NULL;
				}
				memset(buf, 0x00, recv_apk.size + 1);
			}

			if (recv_apk.status == 0x00) {
				memcpy(buf + recv_apk.number * APK_SIZE, recv_apk.buf, APK_SIZE);
			}
			if (recv_apk.status == 0x01) {
				int residue = recv_apk.size - recv_apk.number * APK_SIZE;
				memcpy(buf + recv_apk.number * APK_SIZE, recv_apk.buf, residue);

				//printf("%s\n", buf);
				if (buf != NULL) {
					free(buf);
					buf = NULL;
				}

			}
			if (recv_apk.status == 0x02) {
				// printf("%s\n", "confirm_recv sucess");
			}
		} else {
			printf( "closed: %d\n", cfd);
			close(cfd);
			return NULL;
		}
	}
	return NULL;
}


int tcp_client_start(int cfd) {
	if (tcp_client.pool == NULL) {
		tcp_client.pool = thread_pool_init(1, 1000);
	}
	set_disable_nagle(cfd);
	tcp_client.status = 0x00;
	pthread_t pid;
	pthread_create(&pid , NULL, tcp_client_recv_thread,  (void*)&cfd);
	return true;
}



/**
 * [tcp_client_send 客户端发送数据]
 * @param  fd   [description]
 * @param  buf  [description]
 * @param  size [description]
 * @return      [description]
 */
int tcp_client_send(int fd, char *buf, int size) {
	if (tcp_client.status == 0x01) {
		return -2;
	}
	if (size > get_send_buf_size(fd)) {
		//return -3;
	}

	map<int, struct send_buf_list>::iterator iter = tcp_client.send_buf_map.find(fd);
	if (iter == tcp_client.send_buf_map.end()) {
		struct send_buf sbuf ;
		sbuf.size = size;
		sbuf.status = 0x00;
		sbuf.buf = (char*)malloc(size + 1);
		if (sbuf.buf == NULL) {
			printf("err:malloc\n");
			return -4;
		}
		memset(sbuf.buf, 0x00, size + 1);
		memcpy(sbuf.buf, buf, size);
		struct send_buf_list  slist;
		slist.send_list.push_back(sbuf);
		tcp_client.send_buf_map.insert(pair<int, struct send_buf_list>(fd, slist));
	} else {
		struct send_buf_list *slist = &iter->second;
		if (slist->send_list.size() > SEND_QUEUE_MAX) {
			return -1;
		}
		struct send_buf sbuf ;
		sbuf.size = size;
		sbuf.status = 0x00;
		sbuf.buf = (char*)malloc(size + 1);
		if (sbuf.buf == NULL) {
			printf("err:malloc\n");
			return -4;
		}
		memset(sbuf.buf, 0x00, size + 1);
		memcpy(sbuf.buf, buf, size);
		slist->send_list.push_back(sbuf);
	}

	thread_pool_add_job(tcp_client.pool, client_send_work, (void*)&fd, sizeof(int));
	return true;
}
int tcp_client_end(int fd) {
	tcp_client.status = 0x01;
	sleep(2);
	tcp_client_clear(fd);
	if (tcp_client.pool != NULL) {
		thread_pool_destroy(&tcp_client.pool);
	}
	close(fd);
	return true;
}













