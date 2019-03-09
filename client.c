#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "thread_pool.h"


void *read_call(void *arg)
{
    return NULL;
}


int main()
{
    struct thread_pool * client_pool = thread_pool_init(1, 10);
    struct client_base * cbase = tcp_client_init("127.0.0.1", 8888);
    set_client_thread_pool(cbase, client_pool, read_call);
    tcp_client_start(cbase);
    while (1)
    {
        char buf[100];
        if (gets(buf) != NULL)
        {
            printf("str = %s\n", buf);
#if TCP_QUEEU_TYPE == 0x01
            tcp_client_send(cbase, buf, strlen(buf), 1);
#else
            tcp_client_send(cbase, buf, strlen(buf));
#endif
        }


    }

    sleep(10);
    tcp_client_end(&cbase);
    return 0;
}