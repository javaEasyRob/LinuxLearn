//已检测
//sigemptyset，sigaction忘了很多
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
// #define  __USE_POSIX

#include<semaphore.h>
#include <netinet/in.h>
#include<sys/socket.h>
// #include<netinet/in.h>
#include<arpa/inet.h>
#include<wait.h>
#define NAMESIZE 10
typedef struct stu{
    int id;
    char name[NAMESIZE]; 
}Stu;
void handler()
{  
    while(waitpid(0,NULL,WNOHANG)>0);
    return;
}
int main()
{
    char buf[BUFSIZ];
    signal(SIGPIPE,SIG_IGN);


    // signal(SIGCHLD,handler);//绑定信号
    struct sigaction act;
    act.sa_handler=handler;//设置函数
    sigemptyset(&act.sa_mask);//清空屏蔽码
    act.sa_flags=0;
    sigaction(SIGCHLD,&act,NULL);//绑定信号
    struct sockaddr_in server_addr,client_addr;
    int len=sizeof(client_addr);

    int sfd=socket(AF_INET,SOCK_STREAM,0);//创建套接字
    
    int opt=1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEPORT,(void*)&opt,sizeof(opt));//设置端口复用
    bzero(&server_addr,0);//清空结构体
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//设置ip地址
    server_addr.sin_family=AF_INET;//设置类型
    server_addr.sin_port=htons(8888);//设置端口
    
    bind(sfd,(struct sockaddr*)&server_addr,sizeof(server_addr));//绑定套接字的地址结构;
    listen(sfd,128);//设置监听上限
    int count=0;
    while(1){
        printf("i am waiting client %d\n",count++);
        int cfd;
        while((cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
            continue;
        if(cfd==-1){
            perror("some thing wrong");
            exit(-1);
        }
        // printf("%d",cfd);
        puts("客户端连接成功!");
        int pid=fork();
        if(pid>0){
            // close(cfd);
            continue;
        }else if(pid==0){
            close(sfd);
            while(1){
                Stu stu;
                int getlen;
                puts("im reading");
                if((getlen=read(cfd,&stu,sizeof(stu)))==0){
                    close(cfd);
                    puts("the client have quit");
                    exit(1);
                }
                // printf("%s\n",strerror(errno));
                printf("%d\n",getlen);
                printf("Client: IP:%s\tport:%d\tid:%d\tname:%s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,buf,sizeof(buf)),ntohs(client_addr.sin_port),stu.id,stu.name);
                char str[]="ok";
                write(cfd,str,sizeof(str));
                printf("Server:%s\n",str);
                sleep(1);
            }
            close(cfd);
        }else if(pid<0){
            perror("fork err");
            exit(1);
        }
    }
}