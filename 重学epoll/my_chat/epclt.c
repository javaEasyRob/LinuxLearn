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
#include"ui.h"
#include"c_io.h"
#define PWDSIZE 10
#define NAMESIZE 10
#define MAXLINE 4096
int cfd=-1;

void*toRead(void*arg)
{
    PACK recv_pack;
    while(1){
        readPack(cfd,&recv_pack);
        // else if( recv_pack.msg_kind==MSG_CNT&&strcmp(recv_pack.buf,"收到")==0)
        //     printf("发送成功\n");
    }
}
void*toWrite(void*arg)
{
    char *packSenderName=(char*)arg;
    PACK send_pack;
    strcpy(send_pack.packSender,packSenderName);
    while(1){
        puts("1---私聊\t2---群发");
        char choice=getchar();
        while(getchar()!='\n');
        switch (choice){
            case '1':
                privateChat(&send_pack,cfd,packSenderName);            
                break;
            case '2':
                groupChat(&send_pack,cfd,packSenderName);
            default:
                break;
        }
    } 
    pthread_exit(NULL);
}

int login(char*clientName)
{
    PACK login_Pack,ret_Pack,exit_Pack;
    while(1){
        loginConfirmation(&login_Pack,&ret_Pack,cfd);
        strcpy(clientName,login_Pack.packSender);

        if(ret_Pack.msg_kind==MSG_ACK){
            // printf("%s\n",ret_Pack.buf);
            return 0;
        }else if(ret_Pack.msg_kind==MSG_FAIL){
            // printf("%s",ret_Pack.buf);
            if(exitRequest(&exit_Pack,cfd,clientName))
                return -2;
            else
                continue;
        }else{
            perror("服务器发来了错误的类型");
            return -1;
        }
    }
}

int main()
{
    cfd=connect_init();
    char clientName[NAMESIZE];
    int ret=login(clientName);
    if(ret==-1){
        puts("系统出错啦！");
        exit(EXIT_FAILURE);
    }else if(ret ==-2){
        puts("bye");
        exit(EXIT_SUCCESS);

    }else{
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
}