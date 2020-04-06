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
#include<sys/select.h>
#include<sys/epoll.h>
#include"ui.h"
#include"c_io.h"
#define PWDSIZE 10
#define NAMESIZE 10
#define MAXLINE 4096
int cfd=-1;
int main()
{
    cfd=connect_init();
    while(1){
        int ch=menu();
        switch (ch){
        case 1:
            login();
            break;
        case 2:
            logon();
            break;
        case 3:
            return_exit();
            break;
        default:
            puts("出错了！");
            break;
        }
    }
    return 0;   
}
