#include "client_socket.h"



struct thread_pool* pool = NULL;
int tcp_client_start(const char *IP, int port)
{
	pthread_t pid;
	struct sockaddr_in serveraddr;
	int confd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, IP, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port  = htons(port);
	//3.链接服务器
	connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	pool = thread_pool_init(1, 10);
	return confd;
}

struct socket_info *  set_call_function(int sfd, void* (*msg_call_function)(void *arg), void* (*err_call_function)())
{

	if (msg_call_function == NULL)
	{
		printf("%s\n", "error: set_call_function need set msg_call_function");
		return NULL;
	}
	struct socket_info * sinfo = (struct socket_info*)malloc(sizeof(sinfo));
	sinfo->socket_fd = sfd;
	sinfo->msg_call_function = msg_call_function;
	sinfo->err_call_function = err_call_function;
	pthread_create(&sinfo->pid , NULL, thread_function, (void*)sinfo);
	return sinfo;
}

void* thread_function(void* arg)
{
	struct socket_info * sinfo = (struct socket_info*)arg;
	printf("%d\n", sinfo->socket_fd);
	char *buf = NULL;
	while (1)
	{
		struct data_apk apk;
		int len = read(sinfo->socket_fd, &apk, sizeof(struct data_apk));
		if (buf == NULL)
		{
			buf = (char*)malloc(apk.size + sizeof(struct send_buf) + 1);
			memset(buf, 0x00, apk.size + sizeof(struct send_buf) + 1);
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

void* send_work(void* arg)
{
	struct send_buf * test_apk = (struct send_buf*)arg;
	printf("%s\n", test_apk->buf);
	struct data_apk apk;
	apk.size = test_apk->size + sizeof(struct send_buf);
	int count = apk.size / APK_SIZE;
	int iResult = 0;
	for (int i = 0; i < count; ++i)
	{
		apk.number = i;
		memset(apk.buf, 0x00, sizeof(apk.buf));
		if (i * APK_SIZE + APK_SIZE < apk.size) {
			memcpy(apk.buf, (char*)test_apk + i * APK_SIZE, APK_SIZE);
		} else {
			int opt = i * APK_SIZE;
			memcpy(apk.buf, (char*)test_apk + opt, apk.size - opt);
		}
		if (i == count - 1)
		{
			apk.status = 0x01;
		} else {
			apk.status = 0x00;
		}
		write(test_apk->socket_fd, &apk, sizeof(apk));
	}
	return NULL;
}

void send_msg(int fd, char *buf, int size)
{
	printf("%s\n", buf );
	struct send_buf *sbuf = (struct send_buf * )malloc(sizeof(struct send_buf) + size);
	sbuf->socket_fd = fd;
	sbuf->size =  size;
	memset(&sbuf->buf, 0x00, size);
	memcpy(&sbuf->buf, buf, size);
	thread_pool_add_job(pool, send_work, sbuf, size);
}


void tcp_client_end(struct socket_info **sinfo)
{
	free(*sinfo);
	(*sinfo) = NULL;
}








