//have check

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
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
#include<wait.h>
#include<pthread.h>
#define NAMESIZE 10
typedef struct stu{
    int id;
    char name[NAMESIZE];    
}Stu;
typedef struct s_info{
    struct sockaddr_in client_addr;
    int connfd;
}s_info;
//一个拥有地址结构和通信套接字描述符的结构体，用来从arg中分离出参数


void*func(void*arg)
{   
    struct s_info* info=(s_info*)arg;
    int connfd=info->connfd;
        // info->client_addr.sin_addr
    Stu stu;
    while(1){
        // Stu stu;
        int readlen;
        while((readlen=read(connfd,&stu,sizeof(stu)))==-1&&errno==EINTR)
            continue;
        if(readlen==0){
            close(connfd);
            pthread_exit(NULL);
        }
        char buf[BUFSIZ];
        printf("Client: IP:%s\tport:%d\tid:%d\tname:%s\n",inet_ntop(AF_INET,&info->client_addr.sin_addr,
        buf,sizeof(buf)),ntohs(info->client_addr.sin_port),stu.id,stu.name);
        char str[]="ok";
        write(connfd,str,sizeof(str));
        printf("Server:%s\n",str);
        sleep(1);
    }
}
int main()
{
    pthread_t pid;
    int cfd,sfd;
    s_info arginfo;
     struct sockaddr_in client_addr;
    int len=sizeof(client_addr);
    struct sockaddr_in server_addr;
    s_info info;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8888);
    sfd=socket(AF_INET,SOCK_STREAM,0);
    bind(sfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    listen(sfd,128);
    int count=0;
    while(1){
        while((cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
            continue;
        info.client_addr =client_addr;
        info.connfd=cfd;
        pthread_create(&pid,NULL,func,(void*)(&info));
        pthread_detach(pid);
            count++;
    }
    close(cfd);
    close(sfd);
}