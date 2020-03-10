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

struct people
{
    int id;
    char name[4];
};
void* tfn(void*arg)
{
    struct people*ar=    (struct people*)arg;
    ar->id=1;
    strcpy(ar->name,"name");
    return (void*)ar;
}
int main()
{   
    struct people peo;
    struct people *returnval;
    pthread_t p;
    pthread_create(&p,NULL,tfn,(void*)&peo);
    pthread_join(p,(void*)&returnval);
    printf("%d\t%s\n",returnval->id,returnval->name);
    pthread_exit(NULL);
}