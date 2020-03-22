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

        pid=fork();
        if(pid==0)
            break;
    
    }
    //阻塞版本回收子进程
    // if(i==5){
    //     while((wpid=waitpid(-1,NULL,0))!=-1)
    //     // printf("%d\n",tempid);
    //     printf("i am father%d  ,my sun %d die \n",getpid(),wpid);
    // }
    //非阻塞版本
    if(i==5){
        while((wpid=waitpid(-1,NULL,WNOHANG))!=-1){
            if(wpid>0){
                printf("%d die\n",wpid);                
            }else if(wpid==0){
                printf("继续等\n");
                sleep(1);
                continue;
            }
            
        }
    }
    else{
        sleep(i);
        printf("i am the %d child \n",i);
    }
}