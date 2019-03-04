#include "message_base.h"

message_base::message_base() {

}
message_base::~message_base() {

}
void  message_base::new_accept(int fd) {
	printf("%s\n", "new accept" );
}


void message_base::new_msg(int fd, char *buf, int size) {
	//将数据加入线程中,线程池中分配释放内存
}

void message_base::abnormal(int fd) {
	printf("%s\n", "abnormal");
}