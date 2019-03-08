#ifndef CONFIG_H
#define CONFIG_H


#define COMPILE_TYPE 0x00	//编译方式 0x00:服务端
// #define COMPILE 0x01  0x01:客户端

#define SERVER_READ_TYPE 0x00 //线程池分别读取
// #define SERVER_READ_TYPE 0x01 //主线程读取后,将数据包添加任务到线程池中

#endif