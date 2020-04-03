#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<stdbool.h>
#include <sys/types.h>  
#include"s_io.h"
#define SERV_PORT   8888
extern int g_efd;
int main(int argc,char**argv)
{
    unsigned short port=argc==2?atoi(argv[1]):SERV_PORT;
    struct epoll_event all_events[MAX_EVENTS+1];
    g_efd=epoll_create(MAX_EVENTS+1);

    initlistensocket(g_efd,port);
    puts("连接中....");

    while(1){
        DealEventLoop(all_events);
    }
    return 0;
}