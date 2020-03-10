#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{   pid_t pid;
    int i=0;
    //int count=0;
    for( i=0;i<3;i++){
        pid=fork();
        if(!pid)
            break;
    }

    if(pid==-1){
        perror("fork error");
        exit(1);
    }
     if(pid==0){
        //printf("第%d个孩子：\n",count++);
        printf("i'm  个child,my pid=%d,my parid=%d\n",getpid(),getppid());
    }
     if(pid>0){
        //  sleep(1);
        printf("mychild:%d,myid =%d ,myparid=%d\n",pid,getpid(),getppid());
    }
}