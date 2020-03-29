#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
#include<pthread.h>
#include<semaphore.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/epoll.h>
#include"../head.h"

#define NAMESIZE 10
#define SERV_PORT 8888
#define MAXLINE 4096
#define MSG_ACK 1
#define MSG_IN  2
// #define BUFSIZE 1024
// typedef struct stu{
//     int id;
//     char name[NAMESIZE];
// }Stu;
typedef struct ConnctMsg
{
    int fd;
    // int readlen;
    char Name[NAMESIZE]; 
    char buf[BUFSIZ];
}ConnctMsg;
typedef struct MySelfInfo
{
    char myName[NAMESIZE];
    char myIP[32];
    char myPort[7];
}MySelfInfo;
typedef struct m_Connect
{
    MySelfInfo my_info;
    // ConnctMsg c_msg;
    int servfd;
}m_Connect;


typedef struct PACK{
    int msg_kind;
    MySelfInfo myinfo;
    
}PACK;
typedef struct SerMsg{
    int msg_kind;
    char frname[NAMESIZE];
    char readbuf[BUFSIZ];  
}SerMsg;
// typedef struct 
void*toRead(void*arg)
{
    m_Connect*mc=(m_Connect*)arg;
    SerMsg sermsg;
    while(1){
        while(recv(mc->servfd,&sermsg,sizeof(SerMsg),0)==-1){
            continue;
        }
        if(sermsg.msg_kind==MSG_IN)
            printf("%s:%s",sermsg.frname,sermsg.readbuf);
    
        send(mc->servfd,&,,0);
    }
}
void*toWrite(void*arg)
{

}
int main()
{
    // Stu stu;
    char buf[BUFSIZ];
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    puts("connecting...");
    while((connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1){
        // fprintf(stderr,"connect err");
        // exit(1);
        // printf("connecting ...\n");
        // sleep(1);
    }
    puts("connect correct\n");
    while(1){   
        char name[NAMESIZE];
        int id;
        pthread_t pid;
        pthread_create(&pid,NULL,toRead,(void*)());
        printf("client:");
        s_gets(buf,BUFSIZ);
        write(cfd,buf,sizeof(buf));
        read(cfd,buf,sizeof(buf));
        printf("Server:%s\n",buf);
    }
    close(cfd);

}