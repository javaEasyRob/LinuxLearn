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
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    // bind
    struct sockaddr_in serv_addr;
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(8888);
    while(connect(cfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1) {    
           puts("等待服务器连接");
           sleep(1);
            continue;
        }
    puts("链接成功");
    while(1)
    {
        Stu stu;
        stu.id=666;
        puts("你的名字:");
        s_gets(stu.name,NAMESIZE);
        printf("client:%s,%d\n",stu.name,stu.id);
        puts("开始传数据啦！！");
        write(cfd,&stu,sizeof(stu));
        char serv_msg[BUFSIZ];
        read(cfd,serv_msg,sizeof(serv_msg));
        printf("server:%s\n",serv_msg);
        
        sleep(1);
    }
}