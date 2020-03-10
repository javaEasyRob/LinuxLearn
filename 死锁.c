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

pthread_mutex_t mutex1,mutex2;
void* a(void*arg)
{
    pthread_mutex_lock(&mutex1);
    puts("pid1:我有锁1");
    // sleep(1);
    pthread_mutex_lock(&mutex2);
    puts("pid1:我有锁2");
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}
void* b(void*arg)
{
    pthread_mutex_lock(&mutex2);    
    puts("pid2:我有锁2");
    // sleep(1);
    pthread_mutex_lock(&mutex1);   
    puts("pid2:我有锁1");
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);


}
int main()
{
    pthread_t pid1,pid2;
    pthread_mutex_init(&mutex1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    pthread_create(&pid1,NULL,a,NULL);
    pthread_create(&pid2,NULL,b,NULL);
    pthread_join(pid1,NULL);
    pthread_join(pid2,NULL);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    
}