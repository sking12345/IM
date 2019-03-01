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
#include<errno.h>
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

    const char*str1 = "asdkxkxk1123xxxddd";

    struct data_apk apk;

    struct test_apk* test_apk = (struct test_apk*)malloc(sizeof(struct test_apk) + strlen(str1) + 1);
    memset(test_apk, 0x00, sizeof(struct test_apk) + strlen(str1) + 1);
    test_apk->test = 100;
    memcpy(test_apk->buf, str1, strlen(str1));
    // printf("%ld\n", sizeof(struct test_apk) + strlen(str1));
    printf("%s\n", test_apk->buf);

    int opt;
    socklen_t len1 = sizeof(int);
    if ((getsockopt(confd, SOL_SOCKET, SO_SNDBUF, (char*)&opt, &len1)) == 0) {
        printf("SO_KEEPALIVE Value: %d/n", opt);
    }

    // memset(test_apk.buf, 0x00, sizeof(test_apk.buf));
    // memcpy(test_apk.buf, "dd", 2);

    // char * tt = (char *)&test_apk;
    // printf("%ld\n", sizeof(struct test_apk));

    apk.size = sizeof(struct test_apk) + strlen(str1) ;
    int count = apk.size / APK_SIZE;
    int iResult = 0;
    for (int i = 0; i < count; i++)
    {
        apk.number = i;
        memset(apk.buf, 0x00, sizeof(apk.buf));
        if (i * APK_SIZE + APK_SIZE < apk.size)
        {
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
        printf("%d---%s\n",  i * APK_SIZE, apk.buf);
        // write(confd, &apk, sizeof(apk));
        iResult = send(confd, &apk, sizeof(apk), 0);

    }
    free(test_apk);
    test_apk = NULL;
    // memset(apk.buf, 0x00, sizeof(apk.buf));
    // memcpy(apk.buf, "ddd", 3);

    //4.请求服务器处理数据
    // write(confd, &apk, sizeof(apk));

    //5.关闭socket
    close(confd);
    return 0;
}









