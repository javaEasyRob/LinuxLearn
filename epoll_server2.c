









/*failed*/

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
// typedef struct stu{
//     int id;
//     char name[NAMESIZE];
// }Stu;

int main()
{
    // Stu stu;
    // int stu;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr,client_addr;
    addr.sin_port=htons(8887);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(sfd,128);
    int epfd=epoll_create(1024);
    struct epoll_event tep,ep[1024];
    epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&tep);
    while(1){
        int len=sizeof(client_addr);
        int cfd;
        puts("accept");
        
        int res=epoll_wait(epfd,ep,1024,-1);
        printf("res=%d\n",res);
        for(int i=0;i<res;i++){
            if(ep[i].data.fd==sfd){
                while((cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
                    continue;
                char clientip[BUFSIZ];
                printf("Client:  ip:%s\tport :%d\t",
                inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
                ntohs(client_addr.sin_port));
                int flag=fcntl(cfd,F_GETFL);
                flag|=O_NONBLOCK;//             客户cfd 的read无阻塞
                fcntl(cfd,F_SETFL,flag);
                tep.data.fd=cfd;
                tep.events=EPOLLIN|EPOLLET;//ET
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tep);
                printf("epoll wait begin\n");
            }
            if(ep[i].data.fd>=0){
                char buf[1024];
                do{
                    while ((len=read(ep[i].data.fd,buf,sizeof(buf)/2))>0){   //注意这里的大小不足
                        puts("reading...");
                        // printf("len=%d\n",len);
                        // int  a=write(STDOUT_FILENO,buf,len);
                        // printf("%d",a);
                        printf("%s\n",buf);
                    }
                    printf("len=%d\n",len);
                    if(len==0){
                        puts("the client gone");
                        epoll_ctl(epfd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);
                        close(ep[i].data.fd);
                        break;
                    }
                    printf("len=%d\n",len);

                }while (len==-1&&errno==EINTR);  
                puts("out");     
            }
        }
    }        
}