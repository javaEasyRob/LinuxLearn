#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
int main()
{
    pid_t pid;
    int ret;
    pid=fork();
    if(pid>0)
    exit(0);
    ret=chdir("/tmp");
    if(ret==-1){
        perror("chdir");
    }
    umask(0022);
    // close(STDOUT_FILENO);
    close(STDIN_FILENO);
    int fd=open("/dev/null",O_RDWR);
    dup2(fd,STDOUT_FILENO);
    dup2(fd,STDERR_FILENO);
    while(1);   
}