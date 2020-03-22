#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<wait.h>
int main()
{
    pid_t pid,wpid,pid2;
    int status,tempid=0,i;
    for (i = 0; i < 5; i++)
    {
        // if((pid=fork())==0){
        //     if(i==2){
        //     tempid=pid2=getpid();
        //     printf("%d\n",tempid);
        //    }
        // break;
        // }
        //这么写的话，temppid是子进程的变量，子进程县结束的话，父进程的变量就不是那个值了,及初始值
        pid=fork();
        if(pid==0)
            break;
        if(i==2){//父
            tempid=pid;
        }
    }
    if(i==5){
        wpid=waitpid(tempid,NULL,0);
        printf("%d\n",tempid);
        printf("i am father%d  ,my sun %d die i die\n",getpid(),wpid);
    }else{
        sleep(i);
        printf("i am the %d child \n",i);
    }
}