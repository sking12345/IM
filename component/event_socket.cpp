#include "event_socket.h"
#include <string.h>


singleton *singleton::ptr = NULL;
pthread_mutex_t singleton::mutex;

singleton::singleton()
{
	pthread_mutex_init(&mutex, NULL);


}
singleton *singleton::get_instance()
{
	if (ptr == NULL)
	{
		pthread_mutex_lock(&mutex);
		if (ptr == NULL)
		{
			ptr = new singleton();
			pthread_mutex_unlock(&mutex);
		}
	}
	return ptr;
}
//记录设置监听端口的fd
void singleton::set_listener(int listener_fd)
{
	this->listener_fd = listener_fd;
}
/**
 * [singleton::new_connect 新的客户端连接]
 * @param sfd [description]
 */
void singleton::new_connect(int sfd) {
	struct connect_list clist;
	clist.status = false;
	clist.time = time(NULL);
	this->client_fd_map.insert(pair<int, struct connect_list>(listener_fd, clist));
}
/**
 * [singleton::read_connect 接收到客户端发送的数据]
 * @param sfd [description]
 * @param apk [description]
 */
void singleton::read_connect(int cfd, struct data_apk apk)
{

	map<int, struct apk_list>::iterator iter = this->apk_list_map.find(cfd);
	if (iter == this->apk_list_map.end())
	{
		struct apk_list apk_list;
		apk_list.sockfd = cfd;
		apk_list.list.push_back(apk);
		this->apk_list_map.insert(pair<int, struct apk_list>(cfd, apk_list));
	} else {
		struct apk_list *apk_list = &iter->second;
		apk_list->list.push_back(apk);
	}
	if (apk.status == 0x01)
	{
		printf("%d\n", apk.size);
		char *buf = (char *)malloc(apk.size + 1);
		memset(buf, 0, apk.size + 1);
		map<int, struct apk_list>::iterator iter1 =  this->apk_list_map.find(cfd);
		struct apk_list *apk_list1 = &iter1->second;
		list<struct data_apk>::iterator iter_list;
		for (iter_list = apk_list1->list.begin(); iter_list != apk_list1->list.end(); iter_list++)
		{
			memcpy(buf + iter_list->number * APK_SIZE, iter_list->buf, APK_SIZE);
		}
		// struct test_apk* tsk_apk = (struct test_apk*)buf;
		struct test_apk* tsk_apk = NULL;
		tsk_apk = (struct test_apk*)buf;

		printf("%d -- %s\n", tsk_apk->test, tsk_apk->buf);
		apk_list1->list.clear();
		// printf("%d----%s\n", tsk_apk->test, "xx");
		// free(tsk_apk);
		// tsk_apk = NULL;
		// printf("%d\n", test_apk->test);

		// printf("%s\n", buf );
		free(buf);
		buf = NULL;
	}
}

void singleton::abnormal(int sfd)
{
	// map<int, struct apk_list>::iterator iter1 =  this->apk_list_map.find(sfd);
	// struct apk_list *apk_list1 = &iter1->second;
	// apk_list1->list.clear();
	// this->apk_list_map.erase(sfd);
	// this->client_fd_map.erase(sfd);	//删除维护客户端的连接
	printf("%s\n", "close" );
}

void accept_cb(int fd, short events, void* arg)
{
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);

	singleton *singleton_obj = singleton::get_instance();
	singleton_obj->new_connect(fd);


	struct event_base* base = (struct event_base*)arg;

	//仅仅是为了动态创建一个event结构体
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST,
	             socket_read_cb, (void*)ev);

	event_add(ev, NULL);
}
void socket_read_cb(int fd, short events, void *arg)
{
	struct data_apk apk;
	struct event *ev = (struct event*)arg;
	singleton *singleton_obj = singleton::get_instance();
	int len = read(fd, &apk, sizeof(struct data_apk));
	if ( len <= 0 )
	{
		singleton_obj->abnormal(fd);
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {
		singleton_obj->read_connect(fd, apk);
	}
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


int tcp_server_start(int port, int listen_num) {
	int listener = tcp_server_init(port, listen_num);
	if (listener == -1)
	{
		perror("tcp_server_init error");
	}
	singleton *singleton_obj = singleton::get_instance();
	singleton_obj->set_listener(listener);
	struct event_base * base = event_base_new();
	struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, (void*)base);
	event_add(ev_listen, NULL);
	event_base_dispatch(base);
	return true;
}


























