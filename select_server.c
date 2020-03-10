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
    int maxfd=sfd;
    fd_set temp,reads;
    FD_ZERO(&reads);
    FD_SET(sfd,&reads);
    while(1)
    {
        temp=reads;
        puts("select");
        int ret=select(maxfd+1,&temp,NULL,NULL,NULL);//阻塞,既处理read也处理accept
        printf("%d\n",ret);
        if(ret==-1){
            perror("something wrong");
        }
        int len=sizeof(client_addr);
        if(FD_ISSET(sfd,&temp))
        {
            puts("accept someone");
            int cfd=accept(sfd,(struct sockaddr*)&client_addr,&len);//无阻塞
            FD_SET(cfd,&reads);
            maxfd=cfd>maxfd?cfd:maxfd;
            if(ret==1)//ret==1是指select过滤只有客户端来连接，没有客户端读入，那么continue就可以了;
                continue;
        }
        for (int i = sfd+1; i <=maxfd ; i++)
        {
            if(FD_ISSET(i,&temp))  //注意这里这样的话前面的cfd得下一轮循环才能去读入,此时cfd并不在temp集合中
            {
                int readlen;
                while((readlen=read(i,&stu,sizeof(stu)))==-1&&EINTR==errno)
                    continue;
                if(readlen==0)
                {
                    close(i);
                    FD_CLR(i,&reads);
                }
                char clientip[BUFSIZ];
                printf("Client:  ip:%s\tport :%d\t",
                inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
                ntohs(client_addr.sin_port));
                printf("Stu id=%d\tname=%s\n",stu.id,stu.name);
                char str[]="ok";
                printf("Server:%s\n",str);
                write(i,str,sizeof(str));
                sleep(1);
            } 
        }
    }        
    close(sfd);
}