#ifndef CONFIG_H
#define CONFIG_H


#define COMPILE_TYPE 0x00	//编译方式 0x00:服务端
// #define COMPILE 0x01  0x01:客户端

#define SERVER_READ_TYPE 0x00 //读取完数据后添加到线程池任务中
// #define SERVER_READ_TYPE 0x01 //读取完数据后直接调用回调函数

#define TCP_APK_SIZE 1400

#endif