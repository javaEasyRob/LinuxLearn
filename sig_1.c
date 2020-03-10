#include <stdio.h>
// #include <bits/sigaction.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
#include<wait.h>
#include<signal.h>
#include<error.h>
// #include<sys/wait.h>
 void catch_child(int signo)
 {
     pid_t wpid;
     while ((wpid=wait(NULL))!=-1){
         printf("-----------catch child %d\n",wpid);
     }
     if(errno==EINTR){
         printf("1\n");
     }
     return ;
 }
 int main()
 {
     sigset_t set;
     sigemptyset(&set);
     sigaddset(&set,SIGCHLD);
     sigprocmask(SIG_BLOCK,&set,NULL);
     pid_t pid;
     int i;
     for ( i = 0; i < 15; i++)
     {
         if((pid=fork())==0)
            break;
     }
     if(i==15){
         
         struct sigaction act;
         act.sa_handler=catch_child;
         sigemptyset(&act.sa_mask);
         act.sa_flags=0;
        sigaction(SIGCHLD,&act,NULL);
        sigprocmask(SIG_UNBLOCK,&set,NULL);
        printf("im parent .pid=%d\n",getpid());
        while(1);
     }else{
         sleep(1);
         printf("i m child pid =%d\n",getpid());
     }
 }