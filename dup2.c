#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc,char**argv)
{
    int fd1=open(argv[1],O_RDWR);
    int fd2=open(argv[2],O_RDWR);
    printf("%d%d",fd1,fd2);
    int fdret=dup2(fd1,fd2);
    printf("fdret=%d\n",fdret);
    int ret=write(fd2,"12313",5);
    printf("ret=%d",ret);
}