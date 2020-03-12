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
// #include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/epoll.h>
#define NAMESIZE 10
typedef struct stu{
    int id;
    char name[NAMESIZE];
}Stu;

int main()
{
    Stu stu;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr,client_addr;
    addr.sin_port=htons(8888);
    addr.sin_family=AF_INET;
    
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(sfd,128);
    int epfd=epoll_create(1024);
    struct epoll_event tep,ep[1024];
    tep.data.fd=sfd;
    tep.events=EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&tep);
    while(1)
    {
        int ret=epoll_wait(epfd,ep,1024,-1);
        for (int i = 0; i < ret; i++)
        {
            if(ep[i].data.fd==sfd){
                    puts("accept someone");
            int cfd;
            int len=sizeof(client_addr);
            while((cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
                continue;
            tep.data.fd=cfd;
            tep.events=EPOLLIN;
            epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tep);
            }else{
                int readlen;
                while((readlen=read(ep[i].data.fd,&stu,sizeof(stu)))==-1&&EINTR==errno)
                    continue;
                if(readlen==0){//用户在这里退出
                    puts("客户退出");
                    close(ep[i].data.fd);
                    epoll_ctl(epfd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);
                    continue;
                }else if(readlen>0){
                char clientip[BUFSIZ];
                printf("Client:  ip:%s\tport :%d\t",
                inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
                ntohs(client_addr.sin_port));
                printf("Stu id=%d\tname=%s\n",stu.id,stu.name);
                char str[]="ok";
                printf("Server:%s\n",str);
                write(ep[i].data.fd,str,sizeof(str));
                sleep(1);
                }
            }   
        }
    }        
    close(sfd);
}