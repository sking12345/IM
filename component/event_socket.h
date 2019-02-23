#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(bufferevent* bev, void* arg);
void event_cb(struct bufferevent *bev, short event, void *arg);
int tcp_server_init(int port, int listen_num);


#endif