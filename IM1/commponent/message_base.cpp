#include "message_base.h"

message_base::message_base() {

}
message_base::~message_base() {

}
void  message_base::new_accept(int fd,) {
	printf("%s\n", "new accept" );
}


void message_base::new_msg(int fd, char buf, int size) {

}



void message_base::abnormal(int fd) {
	printf("%s\n", "abnormal");
}