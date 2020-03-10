#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
int main(int argc,char**argv)
{
    struct stat buf; 
    stat(argv[1],&buf);
    printf("%ld",buf.st_size);
    mode_t mode=buf.st_mode;
    if( S_ISREG(mode))printf("y");    
    
}