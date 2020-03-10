#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(int argc,char*argv[])
{
    link(argv[1],argv[2]);
    unlink(argv[1]);
}