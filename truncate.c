#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
int main(int argc,char*argv[])
{
    int ret=truncate("truncate.txt ",250);
    printf("%d",ret);
}
//失败