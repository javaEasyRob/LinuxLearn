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
int main()
{
    struct sockaddr_in servaddr,clientaddr;
    socklen_t cliaddr_len=sizeof(clientaddr);
    int listenfd,connfd;
    char buf[NAMESIZE],line[MAXLINE];
    char str[INET_ADDRSTRLEN];

    listenfd=Socket(AF_INET,SOCK_STREAM,0);//创建套接字
    int opt;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,(void*)&opt,sizeof(opt));

    servaddr.sin_family=AF_INET;//设置类型,ip,端口号
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(SERV_PORT);

    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));//将servaddr(地址)绑定到套接字上。
    listen(listenfd,10);//设置监听上限，但不是真正的监听，监听由accept函数完成



    struct epoll_event event ,ev;
    int res,len;
    int epfd=epoll_create(20);//创建epoll句柄（红黑树树根），size用来告诉内核这个监听的数目一共有多大
    // printf("accepting connections...\n");
    // connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddr_len);
    
    event.events=EPOLLIN|EPOLLET;//连接到达；有数据来临;设置边缘触发
    event.data.fd=listenfd;
    
    
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&event);
    //POLL_CTL_ADD 注册、EPOLL_CTL_MOD 修 改、EPOLL_CTL_DEL 删除
    //将套接字挂到树上
    struct epoll_event events[20];
    for(;;){
        // puts("wait");
        int nfds=epoll_wait(epfd,events,20,0);
        // printf("%d\n",nfds);
        int i;
        for(i=0;i<nfds;i++){
            if(events[i].data.fd==listenfd){
                 connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&cliaddr_len);
                if(connfd==-1){
                    sys_err("connfd");
                }
                int flag=fcntl(connfd,F_GETFL);
                flag|=O_NONBLOCK;
                fcntl(connfd,F_SETFL,flag);
                ev.data.fd=connfd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
                char ip[64]={0};
                printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET,&clientaddr.sin_addr,str,sizeof(str)),
                    ntohs(clientaddr.sin_port));

            }else{
                if(!(events[i].events&EPOLLIN)) {
                    continue;
                }
                char buf[1024]={0};
                // while(( len=recv(events[i].data.fd,buf,sizeof(buf),0))>0){
                //     printf("recv buf:%s\n",buf);
                //     write(events[i].data.fd,buf,len);
                // }
                if(len==-1){
                    if(errno==EAGAIN){
                        printf("\n缓冲区读完\n");
                    }else
                        sys_err("recv");
                }else if(len==0){
                    printf("client disconnected ...\n");
                    int ret=epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,NULL);
                    if(ret==-1){
                        sys_err("epoll_ctl");
                    }
                    close(events[i].data.fd);
                }
                
            }
        }
    }
    close(listenfd);
    return 0;
}