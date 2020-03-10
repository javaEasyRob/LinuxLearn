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
int  main()
{
    int *p;
    int fd=open("sad",O_CREAT|O_RDWR|O_TRUNC,0644);
    ftruncate(fd,sizeof(int));
    int len=sizeof(int);
    p=mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p==MAP_FAILED){
        perror("map wrong");
    }
    close(fd);
    *p=2;
    printf("%d",*p);
    while (1)
    {
        (*p)=(*p)+1;
        sleep(1);
    }
    
    munmap(p,len);
}
