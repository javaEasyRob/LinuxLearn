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
void *tfn(void*arg)
{
    int n=3;
    while (n--)
    {
        printf("thread count %d\n",n);
        sleep(1);
    }
    pthread_exit((void*)1);
}
int main()
{
    pthread_t tid;
    void*tret;
    int err;
#if 0
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&tid,&attr,tfn,NULL);
    pthread_attr_destroy(&attr);
#else 
    pthread_create(&tid,NULL,tfn,NULL);
    pthread_detach(tid);
#endif
    while (1){
        sleep(1);
        err=pthread_join(tid,&tret);
        if(err!=0)
            fprintf(stderr,"thread_join error:%s\n",strerror(err));
        else
            fprintf(stderr,"thread exit code %d\n",(int)tret);
    }
    return 0;
}
    
