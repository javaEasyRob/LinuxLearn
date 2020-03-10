#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<wait.h>
int main()
{
    pid_t pid,wpid;
    int status;
    pid=fork();
    if(!pid){
        printf("i m child %d\n",getpid());
        sleep(10);
        return 21;
    }else if(pid>0){ 
        wpid=wait(&status);
        if(wpid==-1){
            perror("wait error");
            exit(-1);
        }
        if(WIFEXITED(status)){
            printf("child exit with %d\n",WEXITSTATUS(status));
        }
        if(WIFSIGNALED(status)){
            printf("child kill with signal %d\n",WTERMSIG(status));
        }
        printf("%d break\n",wpid);
    }else {
        perror("fork");
    }
}