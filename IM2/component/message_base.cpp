#include "message_base.h"


message_base::message_base() {

}
message_base::~message_base() {

}
void message_base::connect_success(int fd) {
	printf("connect_success:%d\n", fd);
}
void message_base::new_msg(int fd, char *buf, int size) {
	printf("new_msg:%d\n", fd);
}
void message_base::abnormal(int fd) {
	printf("abnormal:%d\n", fd);
}