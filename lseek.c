#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
int main()
{
    int fd,n;
    char buf[4];
    fd=open("lseek.txt",O_RDWR|O_CREAT,0644);
    int size=write(fd,"ads",4);
    // printf("%d",size);
    lseek(fd,0L,SEEK_SET);
    read(fd,buf,4);
    printf("%s",buf);
    close(fd);
}