#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc,char*argv[])
{   pid_t pid;
    
    pid=fork();
   int fd= open("fork小测试.txt",O_RDWR|O_CREAT|O_TRUNC,0644);
    if(pid==-1){
        perror("fork error");
        exit(1);
    }
     if(pid==0){
        //  execl("/bin/ls","/bin/ls","-l",NULL);
        // execl("./a.out","./a.out",NULL);
        dup2(fd,1);
        strcpy(argv[0],"ps");
         execvp("ps",argv);
         perror("execlp");
         exit(1);
    }
     if(pid>0){
         sleep(1);
        printf("mychild:%d,myid =%d ,myparid=%d\n",pid,getpid(),getppid());
    }
    close(fd);
}