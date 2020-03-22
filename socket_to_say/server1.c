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
    struct sockaddr_in serv_addr,client_addr;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(8888);
    bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(lfd,128);
    int client_len=sizeof(client_addr);
    int cfd=accept(lfd,(struct sockaddr*)&client_addr,&client_len);
    while(1)
    {
        Stu stu;
        puts("开始读啦！！");
        int readlen;
        readlen=read(cfd,&stu,sizeof(stu));
        if(readlen==0){
            printf("客户你不爱我啦!");
            exit(0);
        }
        printf("读取成功");
        printf("client:%s %d\n",stu.name,stu.id);
        printf("server:回执消息：");
        char serv_msg[BUFSIZ];
        s_gets(serv_msg,sizeof(serv_msg));
        write(cfd,serv_msg,sizeof(serv_msg));
        sleep(1);
    }

}