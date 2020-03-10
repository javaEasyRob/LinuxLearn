#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
#include<wait.h>
int main()
{
    int var=0;
    int *p;
    pid_t pid;
    int fd=open("temp",O_RDWR|O_CREAT|O_TRUNC,0644);
    if(fd<0){
        perror("open");
        exit(1);
    }
    ftruncate(fd,4);
    p=(int*)mmap(NULL,4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if (p==MAP_FAILED)
    {
        perror("mmap error");
        exit(1);
    }
    close(fd);
    pid=fork();
    if(pid==0){
        *p=7000;
        var=1000;
    }else{
        sleep(1);
        printf("%d------%d",*p,var);
        wait(NULL);
        int ret=munmap(p,4);
        if(ret==-1){
            perror("munmap");
            exit(2);
        }
    }
    
    
}