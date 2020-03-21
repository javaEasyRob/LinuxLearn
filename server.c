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
    int len=sizeof(client_addr);
    char clientip[BUFSIZ];
    int cfd=accept(sfd,(struct sockaddr*)&client_addr,&len);
    printf("client ip:%s     port :%d\n",
    inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
    ntohs(client_addr.sin_port));//client port is admin by system
    int getlen;
    while(1){
        if((getlen=read(cfd,&stu,sizeof(Stu)))==0){
            close(cfd);
            close(sfd);
            return 0;
        }
        printf("Client: Stu    id=%d,name=%s\n",stu.id,stu.name);
        char str[]="ok";
        printf("Server:%s\n",str);
        write(cfd,str,sizeof(str));
        sleep(1);
    }
    close(cfd);
    close(sfd);
}