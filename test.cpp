#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "component/type.h"
#define SERVER_PORT 8000

int main(int argc, char *argv[])
{
    struct sockaddr_in serveraddr;
    int confd, len;
    char ipstr[] = "127.0.0.1";


    //1.创建一个socket
    confd = socket(AF_INET, SOCK_STREAM, 0);
    //2.初始化服务器地址
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    //"192.168.6.254"
    inet_pton(AF_INET, ipstr, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port  = htons(SERVER_PORT);
    //3.链接服务器
    connect(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    struct data_apk apk;
    apk.status = 0x00;
    apk.number = 0x01;
    memset(apk.buf, 0x00, sizeof(apk.buf));
    memcpy(apk.buf, "ddd", 3);

    //4.请求服务器处理数据
    write(confd, &apk, sizeof(apk));

    //5.关闭socket
    close(confd);
    return 0;
}






