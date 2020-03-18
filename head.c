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
#include<semaphore.h>
#include <netinet/in.h>
#include<sys/socket.h>
// #include<netinet/in.h>
#include<arpa/inet.h>
#include<wait.h>
Read(int fd,void*buf,size_t nbytes)
{
    int readlen;
    while((readlen=read(fd,buf,nbytes))==-1&&errno==EINTR)
        continue;
    if(readlen==-1)
        return -1;
}