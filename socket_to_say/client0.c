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
    struct sockaddr_in serv_addr;
    int cfd=socket(AF_INET,SOCK_STREAM,0);   
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    while(connect(cfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        puts("连接中");
        sleep(1);
    }
    while(1)
    {
        Stu stu;
        printf("id:");
        scanf("%d",&stu.id);
        while(getchar()!='\n')
            continue;
        printf("name:");
        // scanf("%s",stu.name);
        s_gets(stu.name,sizeof(stu.name));
        printf("Client: id:%d ,name:%s\n",stu.id,stu.name);
        write(cfd,&stu,sizeof(stu));
        printf("Server:");
        char ser_msg[BUFSIZ];
        int readlen=Read(cfd,ser_msg,sizeof(ser_msg));
        if(readlen==0){
            puts("服务器异常退出");
            close(cfd);
            exit(-1);
        }
        printf("%s\n",ser_msg);
        
    }



}