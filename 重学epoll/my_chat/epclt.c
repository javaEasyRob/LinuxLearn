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
#include"../../head.h"
#define PWDSIZE 10
#define NAMESIZE 10
#define SERV_PORT 8888
#define MAXLINE 4096
#define MSG_EXIT 0
#define MSG_ACK 1
#define MSG_CNT  2
#define MSG_LOGIN 3
#define MSG_FAIL 4
#define MSG_BROADCAST 5
#define EXIT "exit"
int cfd=-1;
// typedef struct ClinetInfo
// {
//     char Name[NAMESIZE];
//     char IP[32];
//     int Port;
// }ClinetInfo;
typedef struct PACK{
    int msg_kind;
    char packSender[NAMESIZE];
    char packRecver[NAMESIZE];
    char buf[BUFSIZ];
}PACK;

void sendPack(int cfd,PACK *ppack)
{
    if(Send(cfd,ppack,sizeof(*ppack),0)==-1){
        puts("粗错了");
        close(cfd);
        printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
        // pthread_exit(NULL);
        exit(EXIT_FAILURE);
    }   
}
void readPack(int cfd,PACK *ppack)
{
    int ret=Recv(cfd,ppack,sizeof(*ppack),0);
        if(ret==0){
            close(cfd);
            puts("服务器挂了");
            exit(EXIT_FAILURE);
        }else if(ret==-1){
            puts("粗错了");
            close(cfd);
            printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }else{    
            if(ppack->msg_kind==MSG_CNT||ppack->msg_kind==MSG_BROADCAST){ 
                printf("%s:%s\n",ppack->packSender,ppack->buf);
            }
        }
}
void*toRead(void*arg)
{
    
    PACK recv_pack;
    while(1){
        readPack(cfd,&recv_pack);
        // else if( recv_pack.msg_kind==MSG_CNT&&strcmp(recv_pack.buf,"收到")==0)
        //     printf("发送成功\n");
    }
    pthread_exit(NULL);
}
void*toWrite(void*arg)
{
    char *packSenderName=(char*)arg;
    PACK send_pack;
    strcpy(send_pack.packSender,packSenderName);
    while(1){
        char choice[3];
        puts("1---私聊\t2---群发");
        s_gets(choice,3);
        if(!strcmp(choice,"1")){
            printf("发送给:");
            s_gets(send_pack.packRecver,NAMESIZE);//后续有列表了用数字
            send_pack.msg_kind=MSG_CNT;
            printf("[输入框]%s:",packSenderName);        
            s_gets(send_pack.buf,BUFSIZ);
            sendPack(cfd,&send_pack);
        }else if(!strcmp(choice,"2")){
            send_pack.msg_kind=MSG_BROADCAST;
            printf("[输入框]%s:",packSenderName);        
            s_gets(send_pack.buf,BUFSIZ);
            sendPack(cfd,&send_pack);
        }
    } 
    pthread_exit(NULL);
}

int login(char*clientName)
{
    PACK loginPack,ret_Pack,exit_Pack;
    // LOGIN loginInfo;
    loginPack.msg_kind=MSG_LOGIN;
    while(1){

        printf("姓名:");
        s_gets(loginPack.packSender,NAMESIZE);
        strcpy(clientName,loginPack.packSender);
        printf("密码:");
        s_gets(loginPack.buf,PWDSIZE);
        sendPack(cfd,&loginPack);
        readPack(cfd,&ret_Pack);
        if(ret_Pack.msg_kind==MSG_ACK){
                printf("%s\n",ret_Pack.buf);
                return 0;
        }else if(ret_Pack.msg_kind==MSG_FAIL){
                puts("你要退出吗?");
                char choice[5];
                s_gets(choice,5);
                if(!strcmp(choice,EXIT)||!strcmp(choice,"quit")){         
                    exit_Pack.msg_kind=MSG_EXIT;
                    sendPack(cfd,&exit_Pack);
                    close(cfd);
                }
        }else{
            perror("服务器粗了小错误");
            return -1;
        }
    }
}
void connect_init()
{
    cfd=Socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    puts("connecting...");
    while((Connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1);

    puts("connect correct\n");
    return ;
}
int main()
{
    connect_init();
    char clientName[NAMESIZE];
    if(login(clientName)==-1){
        puts("系统出错啦！");
        exit(EXIT_FAILURE);
    }
    printf("欢迎登录,%s\n",clientName);
    pthread_t pid1,pid2;
    
    pthread_create(&pid1,NULL,toRead,(void*)(clientName));
    pthread_create(&pid2,NULL,toWrite,(void*)(clientName));
    pthread_detach(pid1);
    pthread_detach(pid2);
    while(1);
    // pthread_join(pid1,NULL);
    // pthread_join(pid2,NULL);
    close(cfd);
}