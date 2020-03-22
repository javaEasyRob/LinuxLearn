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
#include"../head.h"
//ntohl,htonl
//ntohs,htons
//inet_ntop ,inet_pton
#define SERV_PORT 8888
#define NAMESIZE 10
char*s_gets(char*s,int n)
{   char*find;
    char*ret_val;
    ret_val=fgets(s,n,stdin);
    if(ret_val){
        find=strchr(s,'\n');
        if(find){
            *find='\0';
        }else{
            while(getchar()!='\n')
            continue;
        }
    }
    return ret_val;
}
typedef struct stu{
    int id;
    char name[NAMESIZE];
}Stu;
int main()
{
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    int opt;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,(void*)&opt,sizeof(opt));
   
    struct sockaddr_in serv_addr,client_addr;
    // char serv_ip[BUFSIZ];
    // inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    int len=sizeof(serv_addr);
    bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(lfd,128);

    int  client_addr_len=sizeof(client_addr);//这不能是一个随机值，而必须是传入传出参数
    int cfd=Accept(lfd,(struct sockaddr*)&client_addr,&client_addr_len);
    char ip_buf[BUFSIZ];
    printf("ip:%s port:%d\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip_buf,sizeof(ip_buf)),ntohs(client_addr.sin_port));
    char str[BUFSIZ];
    Stu stu;
    while(1)
    {
        int readlen;
        readlen=Read(cfd,&stu,sizeof(stu));
        if(readlen==0){
            puts("客户端退出");
            close(cfd);
            close(lfd);
            exit(-1);
        }
        printf("Client: id:%d  name:%s\n",stu.id,stu.name);
        printf("回执消息:");
        s_gets(str,sizeof(str));
        write(cfd,str,sizeof(str));
        printf("Server:%s\n",str);
    }
}