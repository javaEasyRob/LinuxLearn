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
#define NUM 5
int queue[NUM];
sem_t blank_number,product_number;
void*producer(void*arg)
{
    int i=0;
    while(1){
        sem_wait(&blank_number);
        queue[i]=rand()%1000+1;
        printf("---produce ---%d\n",queue[i]);
        sem_post(&product_number);
        i=(i+1)%NUM;
        sleep(rand()%1);
    }
}
int j=0;
void *consumer(void*arg)
{
    while(1){
    sem_wait(&product_number);
    printf("consumer:%d\n",queue[j]);
    queue[j]=0;
    sem_post(&blank_number);
    j=(j+1)%NUM;
    sleep(rand()%10);
    }
}
int main()
{
    pthread_t pid,cid;
    sem_init(&product_number,0,0);
    sem_init(&blank_number,0,NUM);
    pthread_create(&pid,NULL,producer,NULL);
    for(int i=0;i<3;i++)
    pthread_create(&cid,NULL,consumer,NULL);
    pthread_join(pid,NULL);
    pthread_join(cid,NULL);
    sem_destroy(&product_number);
    sem_destroy(&blank_number);

}