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
#define MSG_ACK 1
#define MSG_CNT  2
#define MSG_LOGIN 3
#define MSG_EXIT 0
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


void*toRead(void*arg)
{
    
    PACK recv_pack;
    while(1){
        while(recv(cfd,&recv_pack,sizeof(recv_pack),0)==-1){
            continue;
        }
        if(recv_pack.msg_kind==MSG_CNT){ 
            printf("%s:%s\n",recv_pack.packSender,recv_pack.buf);
        }
        // else if( recv_pack.msg_kind==MSG_CNT&&strcmp(recv_pack.buf,"收到")==0)
        //     printf("发送成功\n");
    }
    pthread_exit(NULL);
}
void*toWrite(void*arg)
{
    char *packSenderName=(char*)arg;
    PACK send_pack;
    send_pack.msg_kind=MSG_CNT;
    strcpy(send_pack.packSender,packSenderName);
    while(1){
        puts("发送给：");
        s_gets(send_pack.packRecver,NAMESIZE);//后续有列表了用数字
        // send_pack.buf;
        printf("%s:",packSenderName);        
        s_gets(send_pack.buf,BUFSIZ);
        while(send(cfd,&send_pack,sizeof(send_pack),0)<0)
            continue;   
    }
    pthread_exit(NULL);
}

int login(char*clientName)
{
    PACK loginPack,ret_Pack;
    // LOGIN loginInfo;
    loginPack.msg_kind=MSG_LOGIN;
    while(1){

        printf("姓名:");
        s_gets(loginPack.packSender,NAMESIZE);
        printf("密码:");
        s_gets(loginPack.buf,PWDSIZE);
        send(cfd,&loginPack,sizeof(loginPack),0);
        recv(cfd,&ret_Pack,sizeof(ret_Pack),0);
        if(ret_Pack.msg_kind==MSG_LOGIN)
        {
                printf("%s\n",ret_Pack.buf);
            if(strcmp(ret_Pack.buf,"登录成功")==0){
                return 0;
            }else{
                puts("你要退出吗?");
                char choice[2];
                s_gets(choice,2);
                
                if(!strcmp(choice,EXIT)){         
                    loginPack.msg_kind=MSG_EXIT;
                    send(cfd,&loginPack,sizeof(loginPack),0);
                    close(cfd);
                }
            }
        }else{
            perror("粗了小错误");
            // exit(EXIT_FAILURE);
            return -1;
        }
    }
}
void connect_init()
{
    cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    puts("connecting...");
    while((connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1){
    }
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