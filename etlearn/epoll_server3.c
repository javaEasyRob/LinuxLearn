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

#define NAMESIZE 10
#define SERV_PORT 9000

int main()
{
    struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddr_len;
    int listenfd,connfd;
    char buf[NAMESIZE];
    char str[INET_ADDRSTRLEN];
    int efd;

    listenfd=socket(AF_INET,SOCK_STREAM,0);//创建套接字
    
    servaddr.sin_family=AF_INET;//设置类型,ip,端口号
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(SERV_PORT);

    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));//将servaddr(地址)绑定到套接字上。
    listen(listenfd,20);//设置监听上限，但不是真正的监听，监听由accept函数完成
    struct epoll_event event ;
    struct epoll_event resevent[10];
    int res,len;
    efd=epoll_create(10);//创建epoll句柄（红黑树树根），size用来告诉内核这个监听的数目一共有多大
    printf("accepting connections...\n");
    cliaddr_len=sizeof(cliaddr);
    connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddr_len);
    printf("recieved from %s at PORT %d\n",
                inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)),
                ntohs(cliaddr.sin_port));
    
    event.events=EPOLLIN|EPOLLET;//连接到达；有数据来临;设置边缘触发
    event.data.fd=connfd;
    epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&event);
    //POLL_CTL_ADD 注册、EPOLL_CTL_MOD 修 改、EPOLL_CTL_DEL 删除
    //将通信套接字挂到树上

    while(1){
        printf("epollwaiting...\n");   
        int ret=epoll_wait(efd,resevent,10,-1);//-1代表阻塞，阻塞监听客户端相应的操作到来。resevent 是传出参数，保存的是满足的事件
        printf("res=%d\n",ret);//满足的个数
        
        //本例只监听连接套接字的带来的写事件
            if(resevent[0].data.fd==connfd){
                if((len=read(connfd,buf,sizeof(buf)/2))==0){
                    epoll_ctl(efd,EPOLL_CTL_DEL,connfd,NULL);
                    close(connfd);
                    puts("客户离开");
                }else if(len==-1&&errno==EINTR){
                    continue;
                }else{
                printf("client:%s\n",buf);
                write(connfd,buf,strlen(buf)+1);
                printf("server:%s\n",buf);
                }
            }
    }
}