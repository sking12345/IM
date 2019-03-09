#ifndef CONFIG_H
#define CONFIG_H


// #define COMPILE_TYPE 0x00	//编译方式 0x00:服务端
#define COMPILE_TYPE 0x01  //客户端

#define SERVER_READ_TYPE 0x00 //读取完数据后添加到线程池任务中
// #define SERVER_READ_TYPE 0x01 //读取完数据后直接调用回调函数

//有数据读取的回调方式
#define READ_CALL_TYEP 0x00	//调用线程池,线程回调某个回调函数
// #define READ_CALL_TYEP 0x01	//直接回调某个函数


//通信模式配置
#define TCP_APK_SIZE 1400
#define TCP_QUEEU_TYPE 0x00 //tcp 是否采用队列
// #define TCP_QUEEU_TYPE 0x01 //tcp 是否采用队列
#define TCP_SEND_MAX_QUEUE 10	//发送数据的最大队列数
// #define TCP_SEND_TYPE false //发送数据是否采用发送队列

//send 发送数据方式
#define SEND_DATA_TYPE 0x00	//如果发送失败,则将数据添加到发送队列中
// #define SEND_DATA_TYPE 0x01 //如果发送失败，直接返回error

#endif