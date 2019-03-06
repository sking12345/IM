#include "socket.h"

static struct tcp_server tcp_server;
static struct tcp_client tcp_client;


//消息发送
int send_msg(int fd, int number)
{

	return 0;
}

void* send_msg_work(void* arg)
{
	struct client_msg_list *msg_list = (struct client_msg_list *)arg;
	int confd = msg_list->send_fd;
	char *send_buf = msg_list->buf;
	struct data_apk apk;
	apk.size = msg_list->buf_size;
	int residue = apk.size % APK_SIZE;
	int count = (apk.size - residue) / APK_SIZE;
	if (residue > 0)
	{
		apk.status = 0x00;
		for (int i = 0; i < count; ++i)
		{
			apk.number = i;
			memset(apk.buf, 0x00, sizeof(apk.buf));
			memcpy(apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			write(confd, &apk, sizeof(apk));
		}
		apk.status = 0x01;
		memset(apk.buf, 0x00, sizeof(apk.buf));
		memcpy(apk.buf, (char*)send_buf + count * APK_SIZE, residue);
		write(confd, &apk, sizeof(apk));
	} else {
		for (int i = 0; i < count; ++i)
		{
			apk.number = i;
			memset(apk.buf, 0x00, sizeof(apk.buf));
			if (i * APK_SIZE + APK_SIZE < apk.size)
			{
				memcpy(apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			} else {
				int opt = i * APK_SIZE;
				memcpy(apk.buf, (char*)send_buf + opt, apk.size - opt);
			}
			if (i == count - 1)
			{
				apk.status = 0x01;
			} else {
				apk.status = 0x00;
			}
			write(confd, &apk, sizeof(apk));
		}
	}
	sleep(1);
	return NULL;
}

//将需要的消息放入队列中
int server_send_msg(int fd, char * buf, int size)
{
	size_t m_size = sizeof(struct client_msg_list) + size;
	struct client_msg_list *msg_list = (struct client_msg_list*)malloc(m_size);
	memset(msg_list, 0x00, m_size);
	msg_list->buf_size = size;
	msg_list->number = 0;
	msg_list->send_fd = fd;
	msg_list->time = time(NULL);
	memcpy(msg_list->buf, buf, size);

	map<int, struct server_msg_list>::iterator iter = tcp_server.msg_list_map.find(fd);
	if (iter == tcp_server.msg_list_map.end())
	{
		struct server_msg_list server_msg_lst;
		server_msg_lst.send_msg_list.push_back(msg_list);
	} else {
		struct server_msg_list *server_msg_lst = &iter->second;
		server_msg_lst->send_msg_list.push_back(msg_list);
	}
	thread_pool_add_job(tcp_server.pool, send_msg_work, (void*)msg_list, m_size);
	return true;
}
//将需要的消息放入队列中
int client_send_msg(char * buf, int size)
{
	size_t m_size = sizeof(struct client_msg_list) + size;
	struct client_msg_list *msg_list = (struct client_msg_list*)malloc(m_size + 1);
	memset(msg_list, 0x00, m_size);
	msg_list->buf_size = size + 1;
	msg_list->number = 0;
	msg_list->send_fd = tcp_client.socket_fd;
	msg_list->time = time(NULL);
	memcpy(msg_list->buf, buf, size);
	tcp_client.send_msg_list.push_back(msg_list);
	thread_pool_add_job(tcp_client.pool, send_msg_work, (void*)msg_list, m_size);

	return true;
}

int tcp_server_init(int port, int listen_num)
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
	return listener;

error:
	errno_save = errno;
	evutil_closesocket(listener);
	errno = errno_save;
	return -1;
}
int  tcp_server_start(int port, int listen_num, message_base*msg)
{
	if (msg == NULL) {
		printf("%s\n", "error:tcp_server_start message_base don't NULL");
		return -1;
	}
	tcp_server.msg_obj = msg;
	int listener = tcp_server_init(port, listen_num);
	tcp_server.evt_base = event_base_new();
	tcp_server.ev_listen = event_new(tcp_server.evt_base, listener, EV_READ | EV_PERSIST, tcp_server_accept, (void*)tcp_server.evt_base);
	event_add(tcp_server.ev_listen, NULL);
	event_base_dispatch(tcp_server.evt_base);
	return true;

}
void tcp_server_read(int fd, short events, void *arg)
{
	struct data_apk apk;
	struct event *ev = (struct event*)arg;
	int len = read(fd, &apk, sizeof(struct data_apk));
	if ( len <= 0 ) {
		tcp_server.msg_obj->abnormal(fd);
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {
		if (apk.status == 0x01) {	//数据发送完毕
			map<int, struct apk_list>::iterator iter = tcp_server.apk_list_map.find(fd);
			if (iter == tcp_server.apk_list_map.end()) {
				tcp_server.msg_obj->new_msg(fd, (char*)&apk.buf, apk.size + 1);
			} else {
				struct apk_list *apk_list = &iter->second;
				list<struct data_apk>::iterator iter_list;
				char *buf = (char*)malloc(apk.size + 1);
				memset(buf, 0x00, apk.size + 1);
				for (iter_list = apk_list->list.begin(); iter_list != apk_list->list.end(); iter_list++) {
					memcpy(buf + iter_list->number * APK_SIZE, iter_list->buf, APK_SIZE);
				}
				memcpy(buf + apk.number * APK_SIZE, apk.buf, APK_SIZE);
				tcp_server.msg_obj->new_msg(fd, buf, apk.size + 1);
				free(buf);
				buf = NULL;
			}
		} else {
			map<int, struct apk_list>::iterator iter = tcp_server.apk_list_map.find(fd);
			if (iter == tcp_server.apk_list_map.end()) {
				struct apk_list apk_list;
				apk_list.socket_fd = fd;
				apk_list.time = time(NULL);
				apk_list.list.push_back(apk);
				tcp_server.apk_list_map.insert(pair<int, struct apk_list>(fd, apk_list));
			} else {
				struct apk_list *apk_list = &iter->second;
				apk_list->list.push_back(apk);
			}
		}
	}
	return ;
}
void tcp_server_accept(int fd, short events, void* arg)
{
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);
	tcp_server.msg_obj->new_accept(fd);
	struct event_base* base = (struct event_base*)arg;
	//仅仅是为了动态创建一个event结构体
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST,
	             tcp_server_read, (void*)ev);

	event_add(ev, NULL);

}
void tcp_server_end()
{

}



int tcp_client_start(const char *IP, int port)
{
	struct sockaddr_in serveraddr;
	int confd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, IP, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port  = htons(port);
	//3.链接服务器
	int ret = connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret < 0)
	{
		printf("erro:%s connect fail\n", (char*)IP);
		return false;
	}
	tcp_client.socket_fd = confd;
	tcp_client.last_time = time(NULL);

	return confd;
}

void* thread_function(void* arg)
{
	struct tcp_client * sinfo = (struct tcp_client*)arg;
	char *buf = NULL;
	while (1)
	{
		struct data_apk apk;
		int len = read(sinfo->socket_fd, &apk, sizeof(struct data_apk));
		if (buf == NULL)
		{
			buf = (char*)malloc(apk.size + sizeof(struct tcp_client) + 1);
			memset(buf, 0x00, apk.size + sizeof(struct tcp_client) + 1);
		}
		memcpy(buf + apk.number * APK_SIZE, apk.buf, APK_SIZE);
		if (len <= 0)
		{
			if (sinfo->err_call_function != NULL)
			{
				(*(sinfo->err_call_function))();
			}
			close(sinfo->socket_fd);
			break;
		}
		if (apk.status == 0x01)
		{
			(*(sinfo->msg_call_function))(buf);
		}
	}
	return NULL;
}


int set_client_call_function(int sfd, void* (*msg_call_function)(void *arg), void* (*err_call_function)())
{

	tcp_client.msg_call_function = msg_call_function;
	tcp_client.err_call_function = err_call_function;
	pthread_create(&tcp_client.pid , NULL, thread_function, (void*)&tcp_client);
	return true;
}
int set_server_thread_pool(struct thread_pool **pool)
{
	printf("%s\n", "xx");
	tcp_server.pool = *pool;
	return true;
}

int set_client_thread_pool(struct thread_pool **pool)
{
	tcp_client.pool = *pool;
	return true;
}


void tcp_client_end()
{


}



















