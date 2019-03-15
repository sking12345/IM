#include "socket.h"

/**
 * [send_apk 分包发送数据]
 * @param fd   [description]
 * @param buf  [description]
 * @param size [成功返回0，失败返回失败编码]
 */
/**
 * [send_apk 分包发送数据]
 * @param  fd          [socket fd]
 * @param  buf         [数据]
 * @param  size        [数据大小]
 * @param number       [从第几个分包发送,默认0]
 * @param  send_number [第几个数据包发送失败]
 * @return             [成功返回0，失败返回失败编码]
 */
int send_apk(int fd, char *buf, int data_size, int number, int *send_number) {

	struct apk_buf apk;
	int send_size = sizeof(struct apk_buf);
	apk.size = data_size;
	int residue = data_size % TCP_APK_SIZE;
	if (residue > 0) {
		int count = (data_size - residue) / TCP_APK_SIZE;
		for (int i = number; i < count; ++i) {
			apk.number = i;
			apk.status = APK_NOT_END;
			memset(apk.buf, 0x00, TCP_APK_SIZE);
			memcpy(apk.buf, buf + i * TCP_APK_SIZE, TCP_APK_SIZE);
			int status = send(fd, &apk, send_size, 0);
			if (status < 0) {
				log_print("send apk err2");
				*send_number = i;
				return -1;
			}
		}
		apk.status = APK_END;
		apk.number = count;
		memset(apk.buf, 0x00, TCP_APK_SIZE);
		memcpy(apk.buf, buf + count * TCP_APK_SIZE, residue);
		int status = send(fd, &apk, send_size, 0);
		if (status < 0) {
			*send_number = count;
			log_print("send apk err1");
			return -1;
		}
	} else {
		int count = (data_size / TCP_APK_SIZE) - 1;
		for (int i = number; i < count; ++i) {
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
				*send_number = i;
				return -1;
			}
		}
	}
	return 0;
}
/**
 * [send_confirm 恢复确认接受完整的消息]
 * @param  fd  [description]
 * @param  apk [description]
 * @return     [description]
 */
int send_confirm(int fd, struct apk_buf *apk) {
	struct apk_buf con_apk;
	con_apk.status = APK_CONFIRM;
	send(fd, &con_apk, sizeof(struct apk_buf), 0);
	return 1;
}

/**
 * [save_sended_queue 存放发送后的数据队列]
 */
void save_sended_queue(sended_queue_t*sended_queue, send_queue_t*node) {
	if (sended_queue == NULL || node == NULL) {
		printf("%s\n", "null dd");
		return;
	}

	node->next = NULL;
	//锁住该队列,存入已发送的数据,用于服务器确认接受成功
	pthread_mutex_lock(&(sended_queue->mutex));
	if (sended_queue->sq_head == NULL) {
		sended_queue->sq_head = sended_queue->sq_tail = node;
	} else {
		sended_queue->sq_tail->next = node;
		sended_queue->sq_tail = node;
	}
	sended_queue->queue_num++;
	pthread_mutex_unlock(&(sended_queue->mutex));
}



#if COMPILE_TYPE == 0x00

/**
 * [tcp_server_new_connect 新的连接数据,后期对新连接的处理]
 * @param sbase [description]
 */
void tcp_server_new_connect(struct server_base* sbase, int fd) {
	sbase->connect_num++;
#if TCP_QUEEU_TYPE == 0x01

#endif
}
void tcp_server_close_connect(struct server_accept* paccept, int fd) {
	event_free(paccept->ev);
	close(fd);
	server_accept_free(&paccept);
}

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
	struct server_base *pserver = (struct server_base *)arg;
	if (pserver->connect_num >= SERVER_MAX_CONNECT_NUM) {
		log_print("Out of connection");
		close(fd);
		return;
	}
	evutil_make_socket_nonblocking(sockfd);
	printf("accept a client %d\n", sockfd);

	tcp_server_new_connect(pserver, sockfd);
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
	struct apk_buf recv_apk;
	int recv_size = sizeof(struct apk_buf);
	int len = read(fd, &recv_apk, recv_size);

	if ( len <= 0 ) {
		printf("close %d\n", fd);
		tcp_server_close_connect(paccept, fd);
		return ;
	}
	//简单数据验证
	if (recv_apk.status != APK_END && recv_apk.status != APK_NOT_END && recv_apk.status != APK_CONFIRM) {
		log_print("recv data error");
		return;
	}
	char cond_buf[sizeof(struct cond_recv)] = {0x00};
	memcpy(&cond_buf, paccept->pserver->cond_recv + sizeof(struct cond_recv)*fd, sizeof(struct cond_recv));
	struct cond_recv *cond_recv = (struct cond_recv *)cond_buf;
	if (cond_recv->status == 0x00) {
		cond_recv->cfd = fd;
		cond_recv->buf = (char*)malloc(recv_apk.size);
		memset(cond_recv->buf , 0x00, recv_apk.size);
		cond_recv->status = 0x01;
		memcpy(paccept->pserver->cond_recv + sizeof(struct cond_recv)*fd, cond_recv, sizeof(struct cond_recv));
	}
	int residue = recv_apk.size - recv_apk.number * TCP_APK_SIZE;
	if (residue > TCP_APK_SIZE) {
		memcpy(cond_recv->buf + recv_apk.number * TCP_APK_SIZE, &(recv_apk.buf), TCP_APK_SIZE);
	} else {
		memcpy(cond_recv->buf + recv_apk.number * TCP_APK_SIZE, &(recv_apk.buf), residue);
	}
	if (recv_apk.status == APK_END) {
		struct server_read *sread = (struct server_read*)malloc(sizeof(struct server_read));
		sread->fd = fd;
		sread->data_size = recv_apk.size;
		sread->ev =  paccept->ev;
		sread->data_buf = (void*)cond_recv->buf;
		sread->arg = paccept->pserver;

#if SERVER_READ_TYPE == 0x00
		thread_add_job(paccept->pserver->thread_pool, paccept->pserver->read_call, (void*)sread, -1, NULL);
#endif
		cond_recv->status = 0x00;
		memcpy(paccept->pserver->cond_recv + sizeof(struct cond_recv)*fd, cond_recv, sizeof(struct cond_recv));

#if SERVER_READ_TYPE == 0x01
		paccept->pserver->read_call(sread);
#endif
#if TCP_SEND_CONFIRM == 0x01
		send_confirm(fd, &recv_apk);
#endif
		return;
	}
}
//接受数据的连接fd
int get_server_read_fd(void *sread) {
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->fd;
	return 1;
}
//接受到的数据大小
int get_server_read_size(void *sread) {
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->data_size;
	return 1;
}
//接受到的数据
void* get_server_read_buf(void *sread) {
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->data_buf;
	return NULL;
}

struct server_base* get_server_base(void *sread) {
	struct server_read * read_t = (struct server_read*)sread;
	return read_t->arg;
}


void free_server_read_buf(void **sread) {
	struct server_read * read_t = (struct server_read*)*sread;
	char * buf = (char*)read_t->data_buf;
	free(buf);
	buf = NULL;
	free(read_t);
	read_t = NULL;

}
/**
 * 服务端发送消息
 */

void * _tcp_send(void *arg) {
	struct server_base * sbase = (struct server_base * )arg;
	return NULL;
}

void tcp_server_confirm(struct server_base * sbase, struct apk_buf * apk) {

	pthread_mutex_lock(&(sbase->sended_queue->mutex));
	if (sbase->sended_queue->sq_head == NULL) {
		return;
	}
	struct send_queue * node = sbase->sended_queue->sq_head;
	sbase->sended_queue->sq_head = sbase->sended_queue->sq_head->next;
	sbase->sended_queue->queue_num--;
	free(node);
	node = NULL;
	pthread_mutex_unlock(&(sbase->sended_queue->mutex));
	thread_add_job(sbase->thread_pool, _tcp_send, sbase, -1, NULL);
}



int tcp_server_send(struct server_base * sbase, int fd, void *buf, int size, int priority) {

	int send_number = 0;
	int send_info = send_apk(fd, buf, size, 0, &send_number);
	if (send_info < 0) {
		printf("%s\n", "server send error");
	}
	return send_info;

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

void set_server_arg(struct server_base * pserver, void*arg) {
	pserver->arg = arg;
}

void set_server(struct server_base * pserver, struct thread_pool * pool, void*arg) {
	pserver->arg = arg;
	pserver->thread_pool = pool;
}

void set_server_call(struct server_base * pserver, void* (*new_accept)(int cfd),
                     void* (*read_call)(void *sread),
                     void* (*abnormal)(int cfd)) {
	pserver->new_accept = new_accept;
	pserver->read_call = read_call;
	pserver->abnormal = abnormal;
}

int tcp_server_start(struct server_base * pserver, int thread_num) {
#if SERVER_READ_TYPE == 0x00
	if (pserver->thread_pool == NULL) {
		log_debug("tcp server start fail,plase set server thread poll", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
#endif
#if TCP_QUEEU_TYPE == 0x01
	pserver->cond_recv = (char*)malloc(sizeof(struct cond_recv) * SERVER_MAX_CONNECT_NUM);
	memset(pserver->cond_recv, 0x00, sizeof(struct cond_recv) * SERVER_MAX_CONNECT_NUM);
#endif

	if (pserver->read_call == NULL) {
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
struct client_base * tcp_client_init(const char *ipstr, int port) {
	int sockfd;

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
	base->fd = confd;

	return base;
}
void set_client_thread_pool(struct client_base * cbase, struct thread_pool * pool, void*(*read_call)(void *cread)) {
	cbase->thread_pool = pool;
	cbase->read_call = read_call;
}


void *client_read_thread(void *arg) {
	struct client_base* cbase = (struct client_base*)arg;
	int fd = cbase->fd;
	while (1) {
		struct apk_buf recv_apk;
		int len = recv(fd, &recv_apk, sizeof(struct apk_buf), 0);
		if ( len <= 0 ) {
			close(fd);
			return NULL;
		}
#if TCP_SEND_CONFIRM == 0x01	//如果采用消息需要确认,接受确认消息后执行
		if (recv_apk.status == APK_CONFIRM) {
			tcp_client_confirm(cbase, &recv_apk);
			continue;
		}
#endif
		if (cbase->status == 0x00) {
			cbase->recv_buf = (char*)malloc(recv_apk.size);
			memset(cbase->recv_buf, 0x00, recv_apk.size);
			cbase->status = 0x01;
		}
		int residue = recv_apk.size - recv_apk.number * TCP_APK_SIZE;
		if (residue > TCP_APK_SIZE) {
			memcpy(cbase->recv_buf + recv_apk.number * TCP_APK_SIZE, &(recv_apk.buf), TCP_APK_SIZE);
		} else {
			memcpy(cbase->recv_buf + recv_apk.number * TCP_APK_SIZE, &(recv_apk.buf), residue);
		}

		if (recv_apk.status == APK_END) {
			cbase->status = 0x00;
			printf("recv server:%s\n", cbase->recv_buf);
#if CLIENT_READ_TYPE == 0x00
			thread_add_job(cbase->thread_pool, cbase->read_call, (void*)cbase->recv_buf, -1, NULL);
#endif
#if CLIENT_READ_TYPE == 0x01
			cbase->read_call(cbase->recv_buf);
#endif
#if TCP_SEND_CONFIRM == 0x01
			send_confirm(fd, &recv_apk);
#endif
			continue;
		}

	}
	return NULL;

}

#if TCP_QUEEU_TYPE == 0x01	//发送消息采用队列机制
void *client_send_thread(void *arg) {
	struct client_base* cbase = (struct client_base*)arg;
	struct send_thread *sthread  = cbase->sthread;
	while (1) {
		pthread_mutex_lock(&(sthread->mutex));
		while (sthread->queue_num == 0) { //队列为空时，就等待队列非空
			pthread_cond_wait(&(sthread->cond), &(sthread->mutex));
		}
		if (sthread->close) {	//关闭线程
			if (sthread->sq_head != NULL) {
				struct send_queue * node;
				struct send_queue * head = sthread->sq_head;
				while (head != NULL) {
					node = head;
					head = head->next;
					free(node);
					node = NULL;
				}
			}
			pthread_mutex_unlock(&(sthread->mutex));
			pthread_cond_signal(&(sthread->close_cond));
			pthread_exit(NULL);
		}
		sthread->queue_num--;
		struct send_queue * snode = sthread->sq_head;
		sthread->sq_head = sthread->sq_head->next;
		pthread_mutex_unlock(&(sthread->mutex));
		int send_number = 0;
		int send_info  = send_apk(snode->fd, snode->buf, snode->size, snode->send_number, &send_number);
		if ( send_info < 0) { //发送失败,将发送失败的数据发到队列前,暂时考虑连接正常的发送失败
			pthread_mutex_lock(&(sthread->mutex));
			snode->send_number = send_number;
			snode->next = sthread->sq_head;
			sthread->sq_head = snode;
			sthread->queue_num++;
			pthread_mutex_unlock(&(sthread->mutex));
		} else {
#if TCP_SEND_CONFIRM == 0x01
			save_sended_queue(cbase->sended_queue, snode);	//存入已发送队列
#endif
		}
	}
}

#endif

int tcp_client_start(struct client_base * cbase) {
#if READ_CALL_TYEP == 0x00	//调用线程池去调用设置的回调函数
	if (cbase->thread_pool == NULL) {
		log_print("plase set thread poll");
		return -1;
	}
#endif
#if TCP_QUEEU_TYPE == 0x01	//发送消息采用队列机制
	cbase->sthread = (struct send_thread*)malloc(sizeof(struct send_thread));
	cbase->sended_queue = (sended_queue_t*)malloc(sizeof(sended_queue_t));
	cbase->sthread->sq_head = NULL;
	cbase->sthread->sq_tail = NULL;
	cbase->sthread->queue_num = 0;
//用于存放发送的数据
	cbase->sended_queue->sq_head = NULL;
	cbase->sended_queue->sq_tail = NULL;
	cbase->sended_queue->queue_num = 0;

	if (pthread_mutex_init(&(cbase->sthread->mutex), NULL) < 0) {
		log_print("pthread_mutex_init fail");
		free(cbase->sthread);
		cbase->sthread = NULL;
		return -1;
	}
	if (pthread_cond_init(&(cbase->sthread->cond), NULL) < 0) {
		log_print("pthread_cond_init fail");
		free(cbase->sthread);
		cbase->sthread = NULL;
		return -1;
	}

	if (pthread_cond_init(&(cbase->sthread->close_cond), NULL) < 0) {
		log_print("pthread_cond_init fail");
		free(cbase->sthread);
		cbase->sthread = NULL;
		return -1;
	}
	pthread_t send_thread;
	pthread_create(&send_thread, NULL, client_send_thread, (void*)cbase);
#endif

	pthread_t read_thread;
	pthread_create(&read_thread, NULL, client_read_thread, (void*)cbase);
	return 1;
}

#if TCP_QUEEU_TYPE == 0x01

void tcp_client_confirm(struct client_base * cbase, struct apk_buf * apk) {
	pthread_mutex_lock(&(cbase->sended_queue->mutex));
	if (cbase->sended_queue->sq_head == NULL) {
		return;
	}
	struct send_queue * node = cbase->sended_queue->sq_head;
	cbase->sended_queue->sq_head = cbase->sended_queue->sq_head->next;
	cbase->sended_queue->queue_num--;
	free(node);
	node = NULL;
	pthread_mutex_unlock(&(cbase->sended_queue->mutex));
	//唤醒发送数据线程，继续发送消息
	pthread_cond_signal(&(cbase->sthread->cond));
}


int tcp_client_send(struct client_base * cbase, char *buf, int size, int priority) {	//发送数据函数
	int _size = sizeof(struct send_queue) + size;
	struct send_queue * squeue = (struct send_queue*)malloc(_size);
	memset(squeue, 0x00, _size);
	squeue->size = size;
	squeue->next = NULL;
	squeue->fd = get_client_fd(cbase);
	squeue->send_number = 0;
	memcpy(squeue->buf, buf, size);
	pthread_mutex_lock(&(cbase->sthread->mutex));
	if (priority == 0x01) {	//优先发送

		if (cbase->sthread->sq_head == NULL) {
			cbase->sthread->sq_head = cbase->sthread->sq_tail = squeue;
		} else {
			squeue->next = cbase->sthread->sq_head;
			cbase->sthread->sq_head = squeue;
		}
	} else {
		if (cbase->sthread->sq_head == NULL) {
			cbase->sthread->sq_head = cbase->sthread->sq_tail = squeue;
		} else {
			cbase->sthread->sq_tail->next = squeue;
			cbase->sthread->sq_tail = squeue;
		}
	}
	cbase->sthread->queue_num++;
	pthread_mutex_unlock(&(cbase->sthread->mutex));
	pthread_cond_signal(&(cbase->sthread->cond));

	return 1;
}
#else
int tcp_client_send(struct client_base * cbase, char *buf, int size) {
	if (send(cbase->fd, buf, size, 0) < 0) {
		log_print("error:send fail");
		return -1;
	}
	return 1;
}
#endif

int get_client_fd(void *cread) {
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->fd;
}
char * get_client_read_buf(void *cread) {
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->data_buf;
}
int get_client_read_size(void *cread) {
	struct client_read *read_info = (struct client_read*)cread;
	return read_info->data_size;
}



int set_client_call(struct client_base * cbase, void* (*read_call)(void *sread),
                    void* (*abnormal)(int cfd)) {
	cbase->read_call = read_call;
	cbase->abnormal = abnormal;
	return 0;
}

void tcp_client_end(struct client_base **cbase) {
	if (*cbase == NULL) {
		return;
	}
	struct send_queue *node = NULL;
	struct send_queue * head = (*cbase)->sthread->sq_head;
	while (head != NULL) {
		node = head;
		free(node);
		node = NULL;
		head = head->next;
	}
	head = (*cbase)->sended_queue->sq_head;
	while (head != NULL) {
		node = head;
		free(node);
		node = NULL;
		head = head->next;
	}
	if ((*cbase)->fd > 0) {
		close((*cbase)->fd);
	}
	free(*cbase);
	*cbase = NULL;
}

#endif















