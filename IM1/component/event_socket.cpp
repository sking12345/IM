#include "event_socket.h"


void accept_cb(int fd, short events, void* arg)
{
	evutil_socket_t sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	sockfd = accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);

	event_sockt *event_sockt_obj = event_sockt::get_instance();
	event_sockt_obj->new_connect(fd, &client);


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
	event_sockt *event_sockt_obj = event_sockt::get_instance();
	int len = read(fd, &apk, sizeof(struct data_apk));
	if ( len <= 0 )
	{
		event_sockt_obj->abnormal(fd);
		close(event_get_fd(ev));
		event_free(ev);
		return ;
	} else {
		event_sockt_obj->read_connect(fd, apk);
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


int tcp_server_start(int port, int listen_num, message_base* msg_obj) {
	int listener = tcp_server_init(port, listen_num);
	if (listener == -1)
	{
		perror("tcp_server_init error");
	}
	event_sockt *event_sockt_obj = event_sockt::get_instance();
	if (msg_obj == NULL)
	{
		event_sockt_obj->set_msg_obj(new message_base());
	} else {
		event_sockt_obj->set_msg_obj(msg_obj);
	}

	event_sockt_obj->set_listener(listener);
	struct event_base * base = event_base_new();
	struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, (void*)base);
	event_sockt_obj->set_listener_event(&ev_listen);
	event_add(ev_listen, NULL);
	event_base_dispatch(base);
	return true;
}



//--------操作类封装
//
event_sockt *event_sockt::ptr = NULL;
pthread_mutex_t event_sockt::mutex;

event_sockt::event_sockt()
{
	pthread_mutex_init(&mutex, NULL);
}

event_sockt *event_sockt::get_instance()
{
	if (ptr == NULL)
	{
		pthread_mutex_lock(&mutex);
		if (ptr == NULL)
		{
			ptr = new event_sockt();
			pthread_mutex_unlock(&mutex);
		}
	}
	return ptr;
}

void event_sockt::new_connect(int listener_fd, struct sockaddr_in* client)
{
	event_sockt *event_sockt_obj = event_sockt::get_instance();
	message_base * msg_obj = event_sockt_obj->get_msg_obj();

	msg_obj->new_message(1, NULL);
	printf("%s\n", "new cfd");
	struct fds_list cfd_list;
	cfd_list.status = false;
	cfd_list.time = time(NULL);
	this->fds_list.insert(pair<int, struct fds_list>(listener_fd, cfd_list));
}
void event_sockt::read_connect(int cfd, struct data_apk apk)
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
			memcpy(buf + iter_list->number * APK_SIZE, iter_list->buf, 1);
		}
		apk_list1->list.clear();
		free(buf);
		buf = NULL;
	}
}
void event_sockt::set_listener(int listener_fd)
{
	this->listener_fd = listener_fd;
}
void event_sockt::abnormal(int cfd)
{
	printf("%s\n", "close" );
	map<int, struct apk_list>::iterator iter1 =  this->apk_list_map.find(cfd);
	if (iter1 == this->apk_list_map.end())
	{
		return;
	}
	struct apk_list *apk_list1 = &iter1->second;
	if (apk_list1->list.empty())
	{
		return;
	}
	apk_list1->list.clear();
	this->apk_list_map.erase(cfd);
	this->fds_list.erase(cfd);	//删除维护客户端的连接

}

void event_sockt::set_listener_event(struct event** ev_listen)
{
	this->ev_listen = *ev_listen;

}

void event_sockt::set_msg_obj(message_base* msg_obj)
{
	this->msg_obj = msg_obj;
}
message_base* event_sockt::get_msg_obj()
{
	return this->msg_obj;
}
event_sockt::~event_sockt()
{
	close(event_get_fd(this->ev_listen));
	event_free(this->ev_listen);
	this->ev_listen = NULL;
}



























