#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{   pid_t pid;
    
    pid=fork();

    if(pid==-1){
        perror("fork error");
        exit(1);
    }
     if(pid==0){
        //  execl("/bin/ls","/bin/ls","-l",NULL);
        // execl("./a.out","./a.out",NULL);
         execlp("ls","ls","-l",NULL);
         perror("execl");
         exit(1);
    }
     if(pid>0){
         sleep(1);
        printf("mychild:%d,myid =%d ,myparid=%d\n",pid,getpid(),getppid());
    }
}