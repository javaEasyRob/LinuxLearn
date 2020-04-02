#ifndef C_IO_H
#define C_IO_H
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#define MSG_EXIT 0
#define MSG_ACK 1
#define MSG_CNT  2
#define MSG_LOGIN 3
#define MSG_FAIL 4
#define MSG_BROADCAST 5

#define EXIT "exit"

#define PWDSIZE 10
#define NAMESIZE 10

#define SERV_PORT 8888

typedef struct PACK{
    int msg_kind;
    char packSender[NAMESIZE];
    char packRecver[NAMESIZE];
    char buf[BUFSIZ];
}PACK;
void sendPack(int cfd,PACK *pack);
void readPack(int cfd,PACK *pack);
void privateChat(PACK*send_pack,int cfd,char*packSenderName);
void groupChat(PACK*send_pack,int cfd,char*packSenderName);
void loginConfirmation(PACK*login_Pack,PACK*ret_Pack,int cfd);
int exitRequest(PACK*exit_Pack,int cfd,char*packSenderName);
int connect_init();
#endif
