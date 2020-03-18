//https://blog.csdn.net/adlatereturn/article/details/104721644
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
void err_thread(int ret,char*str)
{
    if(ret!=0){
        fprintf(stderr,"%s:%s\n",str,strerror(ret));
        pthread_exit(NULL);
    }
}
struct msg{
    int num;
    struct msg *next;
};
struct msg *head=NULL;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t has_data=PTHREAD_COND_INITIALIZER;
void*consumer(void*arg)
{
    while(1){
        struct msg *mp;
        pthread_mutex_lock(&mutex);
        while(head==NULL){    
            printf("顾客:我在等\n");
            pthread_cond_wait(&has_data,&mutex);
        }
        mp=head;
        head=mp->next;
        pthread_mutex_unlock(&mutex); 
        printf("consumer id:%ld,get :%d\n",pthread_self(),mp->num);
        free(mp);
        sleep(rand()%3);  
    }
    return NULL;
}
void*produser(void*arg)
{
    while (1)
    {    
        struct msg*mp=malloc(sizeof(struct msg));
        mp->num=rand()%1000+1;
        printf("produce %d\n",mp->num);
        
        pthread_mutex_lock(&mutex);
        mp->next=head;
        head=mp;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&has_data);
        sleep(rand()%3); 
    }
    return NULL;   
}
int main()
{

    srand(time(NULL));
    int ret;
    pthread_t pid,cid;
    // pthread_mutex_init(&mutex,NULL);
    // pthread_cond_init(&has_data,NULL);
    ret=pthread_create(&pid,NULL,produser,NULL);
    if(ret!=0)
        err_thread(ret,"pthread_create produser error ");
    //pthread_create函数执行成功返回0
    for (int i = 0; i < 3; i++)
    {
        ret=pthread_create(&pid,NULL,consumer,NULL);
        if(ret!=0)
            err_thread(ret,"pthread_create cosumer error ");
    }
        
        //回收线程资源
        pthread_join(pid,NULL);
        pthread_join(cid,NULL);
    return 0;
}