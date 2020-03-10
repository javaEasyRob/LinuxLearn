#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>//fcntl
#include<unistd.h>//STDIN_FILENO
#include<errno.h>
int main()
{
    char buf[10];
    int flag,n;
    flag=fcntl(STDIN_FILENO,F_GETFL);
    if(flag==-1){
        perror("fcntl error");
        exit(1);
    }
    flag|=O_NONBLOCK;
    int ret =fcntl(STDIN_FILENO,F_SETFL,flag);
    if(ret==-1){
        perror("fcntl error");
        exit(1);
    }
tryagain:
    n=read(STDIN_FILENO,buf,10);
    if(n<0){
        if(errno!=EAGAIN){      //，如果你连续做read操作而没有数据可读。此时程序不会阻塞起来等待数据准备就绪返回，read函数会返回一个错误EAGAIN，提示你的应用程序现在没有数据可读请稍后再试。,在VxWorks和Windows上，EAGAIN的名字叫做EWOULDBLOCK。
            perror("read stdin");
            exit(1);
        }
        sleep(3);
        write(STDOUT_FILENO,"try again ",strlen("try again "));
        goto tryagain;
    }
    write(STDOUT_FILENO,buf,n);
    return 0;
}