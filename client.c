#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"


void *read_call(void *arg) {
    return NULL;
}


int main() {
    struct thread_pool * client_pool = thread_pool_init(1, 1000);
    struct client_base * cbase = tcp_client_init("127.0.0.1", 8888);
    if (cbase == NULL) {
        printf("%s\n", "error" );
        return 0;
    }
    set_client_thread_pool(cbase, client_pool, read_call);
    tcp_client_start(cbase);

//     while (1) {
//         char buf[100];
//         if (gets(buf) != NULL) {
//             printf("str = %s\n", buf);
// #if TCP_QUEEU_TYPE == 0x01
//             tcp_client_send(cbase, buf, strlen(buf) + 1, 1);
// #else
//             tcp_client_send(cbase, buf, strlen(buf) + 1);
// #endif
//         }

//     }
    sleep(1);
    for (int i = 0; i < 2000; ++i) {
        const char *buf = "ddddd";
        tcp_client_send(cbase, (char*)buf, strlen(buf) + 1, 1);
    }
    sleep(1);
    tcp_client_end(&cbase);
    printf("%s\n", "end");
    sleep(5);

    return 0;
}