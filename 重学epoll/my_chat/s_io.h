#ifndef S_IO_H
#define S_IO_H
#include<stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<stdbool.h>
#include <sys/types.h>  
#include"head.h"


#define MSG_EXIT 0
#define MSG_ACK 1
#define MSG_CNT  2
#define MSG_LOGIN 3
#define MSG_FAIL 4
#define MSG_BROADCAST 5

#define PWDSIZE 10
#define NAMESIZE 10

#define MAX_EVENTS  1024      //监听上限

typedef struct PACK{
    int msg_kind;
    char packSender[NAMESIZE];
    char packRecver[NAMESIZE];
    char buf[BUFSIZ];
}PACK;

//这个结构体最好只放事件的信息
struct myevent_s{
    int fd;
    uint32_t events;
    void*arg;
    void(*callback)(void*);
    bool status;//1在树上，0不在树上

}my_events[MAX_EVENTS+1];
void recv_send_data(void*arg);
void servlogin(void*arg);
void recvdata(void*arg);
void senddata(void*arg);
void acceptconn(void*arg);
void eventset(struct myevent_s* mv,int fd,void(*callback)(void*),void*arg);
void eventadd(int efd,int event,struct myevent_s*ms);
void eventdel(int efd,struct  myevent_s*ms);
int  ServerSendPack(int cfd,PACK *ppack);
#endif