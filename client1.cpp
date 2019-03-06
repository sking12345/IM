
#include "component/socket.h"
#include <iostream>
using namespace std;

#define PORT 8000


void socket_send_err1(int fd) {
	printf("%s\n", "ddd");
}


void *send_test(int confd) {
	const char *str_buf1 = "nddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnnddddnxd";
	printf("str_buf1:%ld\n", strlen(str_buf1) );
	struct apk send_apk;
	send_apk.size = strlen(str_buf1);
	int residue = send_apk.size % APK_SIZE;
	int count = (send_apk.size - residue) / APK_SIZE;
	char *send_buf = (char*)str_buf1;
	int status;
	if (residue > 0) {
		send_apk.status = 0x00;
		for (int i = 0; i < count; ++i) {
			send_apk.number = i;
			memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
			memcpy(send_apk.buf, (char*)send_buf + i * APK_SIZE, APK_SIZE);
			status = send(confd, &send_apk, sizeof(send_apk), 0);
			if (status <= 0) {
				socket_send_err1(confd);
				return NULL;
			}
		}
		send_apk.number = count;
		send_apk.status = 0x01;
		memset(send_apk.buf, 0x00, sizeof(send_apk.buf));
		memcpy(send_apk.buf, (char*)send_buf + count * APK_SIZE, residue);
		status = send(confd, &send_apk, sizeof(send_apk), 0);
		if (status <= 0) {
			socket_send_err1(confd);
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
			status = send(confd, &send_apk, sizeof(send_apk), 0);
			if (status <= 0) {
				socket_send_err1(confd);
				return NULL;
			}
		}
	}
	return NULL;
}

void* threadpool_function(void* arg) {
	int id = *(int*)arg;
	// printf("%d\n", id );
	// tcp_client_start(id);
	sleep(1);
	for (int ii = 0; ii < 1000; ++ii) {
		printf("%d--%d\n", id, ii);
		send_test(id);

	}
	// sleep(10);

	return NULL;
}



int main() {

	const char *ip = "127.0.0.1";
	int count = 100;
	int *sfd = (int*)malloc(count * sizeof(int));

	for (int i = 0; i < count; ++i) {
		sfd[i] = tcp_client_init(ip, PORT);
		set_disable_nagle(sfd[i]);
		pthread_t pid;
		pthread_create(&pid, NULL, threadpool_function, (void *)&sfd[i]);
	}
	printf("%s\n", "end");
	sleep(15);
	free(sfd);
	sfd = NULL;
	return 0;
}


//malloc: Incorrect checksum for freed object 0x7faa8e601b98: probably modified after being freed.







