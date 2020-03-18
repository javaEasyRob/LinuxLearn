#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
#include<strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
#include<pthread.h>
#include<signal.h>
#include<semaphore.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<wait.h>
#define SERV_PORT 8887
#define NAMESIZE 10
typedef struct stu{
    int id;
    char name[NAMESIZE];
}Stu;
int main()
{
    int cfd;
    int sockfd;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    char clie_IP[BUFSIZ];
    int ret,i;
    struct sockaddr_in serv_addr,clie_addr;
    socklen_t clie_addr_len;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(sockfd,128);
    clie_addr_len=sizeof(clie_addr);
    while(1){
        Stu stu;
        if((ret=recvfrom(sockfd,&stu,sizeof(stu),0,(struct sockaddr*)&clie_addr,&clie_addr_len))==0){
            // puts("client out!");
        }
        
        printf("client 's ip:%s port:%d\n",inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,clie_IP,sizeof(clie_IP)),ntohs(clie_addr.sin_port));
        printf("Client:id:%d,name:%s\n",stu.id,stu.name);
        char string[BUFSIZ]="ok";
        sendto(sockfd,string,sizeof(string),0,(struct sockaddr*)&clie_addr,clie_addr_len);
        printf("Server:%s\n",string);
        
    }
    close(sockfd);
    close(cfd);

}