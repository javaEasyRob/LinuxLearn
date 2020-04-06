#ifndef C_IO_H
#define C_IO_H
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<ctype.h>
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
void*toRead(void*arg);
void*toWrite(void*arg);
void welcome();

int login();
int logon();
void return_exit();

void sendPack(PACK *pack);
void readPack(PACK *pack);
void privateChat();
void groupChat();
int loginConfirmation();
int exitRequest();
int connect_init();
void welcome();

#endif
