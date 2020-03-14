#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<sys/types.h>
#include<stdbool.h>
bool isDir(char*fileName)
{
    struct stat buf;
    
    stat(fileName,&buf);
    unsigned int mode=buf.st_mode;
     if(S_ISDIR(mode)){
         printf("%s是目录\n",fileName);
        return  true;
     }else {
     printf("%s是文件\n",fileName);
     return false;
     }
}
int main()
{
    printf("%d",strcmp("aaa","c"));
    char name[10]="./test";
    if(isDir(name)){
        // printf("yes");
        DIR*d= opendir(name);
        struct dirent*dd;
        while(dd=readdir(d))
        {
            if(!strcmp(dd->d_name,"a.c"))
            {
                printf("wow\n");
                isDir(dd->d_name);
            }
        }
    }
}