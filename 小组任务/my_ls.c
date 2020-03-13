#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<pwd.h>
#include<stdlib.h>
#include<grp.h>
#include<time.h>
#include<sys/types.h>
#include<stdbool.h>
#define FILENAMESIZE 256
#define FILEMAXNUM 256
#define ARGLEN 16
#define ARGNUM 16
#define ARG_NONE 0
#define ARG_AS   1
#define ARG_AM  2
#define ARG_L   4
#define ARG_R   8
char*getLast(char *name);
void transfer(char(*fileNameList)[FILENAMESIZE],int filenum,int arg);
bool isDir(char*fileName);
void analysisArg(char(*arg)[ARGNUM],int argcount,int *mode);
void analysisFileStat(char*fileName);
void followTrace(char*dirName,int arg);
void printFile(char*name,int arg);
void transfer(char(*fileNameList)[FILENAMESIZE],int filenum,int arg);
char*getLast(char*name)
{
    int len=strlen(name);
    int i;
    for ( i = len-1; i >=0 ; i--){
        if(name[i]=='/')
        return name+i+1;   
    }
    return NULL;
}
bool isDir(char*fileName)
{
    struct stat buf;
    if(stat(fileName,&buf)==-1){
        perror("不存在此文件");
        exit(EXIT_FAILURE);
    }
    unsigned int mode=buf.st_mode;
     if(S_ISDIR(mode)){
        //  printf("%s是目录\n",fileName);
        return  true;
     }
     else{
        //  printf("%s是文件\n",fileName);
     return false;
     }
}
void analysisArg(char(*arg)[ARGNUM],int argcount,int *mode)
{
    for (int i = 0; i < argcount; i++)
    {
        for(int j=0;arg[i][j]!='\0';j++){
            if(arg[i][j]=='a'){
                *mode|=ARG_AS;
            }else if(arg[i][j]=='A'){
                *mode|=ARG_AM;
            }else if(arg[i][j]=='l'){
                *mode|=ARG_L;
            }else if(arg[i][j]=='R'){
                *mode|=ARG_R;
            }
        }
    }
}
// ls -arg
void printFile(char*name,int arg)
{
    char* lastName=getLast(name);
    if(((!strcmp(lastName,"."))||(!strcmp(lastName,"..")))&&(!(arg&ARG_AS))){
        // printf("return\n");
        return;
    }
    if(arg&ARG_L){
        
        analysisFileStat(name);
    }else{
        printf("%-10s",lastName);
    }
}
//ls-l
void analysisFileStat(char*fileName)
{
    struct stat buf;
    if(stat(fileName,&buf)==-1){
        perror("不存在此文件夹");
    }
    unsigned int mode=buf.st_mode;

    /*文件类型*/
    if(S_ISLNK(mode)){
        printf("l");
    }else if(S_ISREG(mode)){
        printf("-");
    }else if(S_ISDIR(mode)){
        printf("d");
    }else if(S_ISBLK(mode)){
        printf("b");
    }else if(S_ISFIFO(mode)){
        printf("f");
    }else if(S_ISCHR(mode)){
        printf("c");
    }else{
        printf("some thing wrong in 文件类型\n");
    }
    
    /*文件权限*/
    char c;
    c=mode&S_IRUSR?'r':'-';
    printf("%c",c);
    c=mode&S_IWUSR?'w':'-';
    printf("%c",c);
    c=mode&S_IXUSR?'x':'-';
    printf("%c",c);
    c=mode&S_IRGRP?'r':'-';
    printf("%c",c);
    c=mode&S_IWGRP?'w':'-';
    printf("%c",c);
    c=mode&S_IXGRP?'x':'-';
    printf("%c",c);
    c=mode&S_IROTH?'r':'-';
    printf("%c",c);
    c=mode&S_IWOTH?'w':'-';
    printf("%c",c);
    c=mode&S_IXOTH?'x':'-';
    printf("%c",c);
    printf(" ");
    struct passwd* psd=getpwuid(buf.st_uid);
    struct group*  grp =getgrgid(buf.st_gid);
    printf("%2ld",buf.st_nlink);
    printf("%-8s",psd->pw_name);
    printf("%-8s",grp->gr_name);
    printf("%6ld",buf.st_size);

    char buf_time[32];
    strcpy(buf_time,ctime(&buf.st_ctime));
    buf_time[strlen(buf_time)-1]='\0'; //去除换行
    printf("    %s  ",buf_time);
        char* lastName=getLast(fileName);

    printf("%s\n",lastName);

}
//跟踪目录
void followTrace(char*dirName,int arg)
{
    printf(":%s\n",dirName);
    strncat(dirName,"/",2);
    DIR*dp;
    int filenum=0;
    struct dirent *sdp;
    char fileNameList[FILEMAXNUM][FILENAMESIZE];
    dp=opendir(dirName);
    while (sdp=readdir(dp)){
            sprintf(fileNameList[filenum++],"%s%s",dirName,sdp->d_name);
    }
    closedir(dp);
    transfer(fileNameList,filenum,arg);
    
}
//中转站
void transfer(char(*fileNameList)[FILENAMESIZE],int filenum,int arg)
{
    for(int i=0;i<filenum;i++){    
        printFile(fileNameList[i],arg);  
        // sleep(1);      
    }
    for(int i=0;i<filenum;i++){    
        if(arg&ARG_R){
             char *lastName=getLast(fileNameList[i]);
            if(isDir(fileNameList[i])&&strcmp(lastName,".")&&strcmp(lastName,"..")){
                printf("即将跟踪%s\n",fileNameList[i]);
                followTrace(fileNameList[i],arg); 
            }
        }
    }
}

int main(int argc,char**argv)
{
    char path[FILENAMESIZE];
    char NameList[FILEMAXNUM][FILENAMESIZE];
    char Arg[ARGNUM][ARGLEN];
    DIR*dp;
    struct dirent *sdp[FILEMAXNUM];

    int argnum=0;
    int filenum=0;

    /*分离文件和参数*/
    /*默认是当前文件夹*/
    
        for (int i = 1; i < argc; i++){
            if(argv[i][0]!='-'){
                strncpy(NameList[filenum++],argv[i],FILENAMESIZE);
                }else{
                strncpy(Arg[argnum++],argv[i],ARGLEN);
            }
        }
        if(filenum==0){
            strcpy(NameList[filenum++],".");
        }

    /*分析参数*/
    int arg=0;
    analysisArg(Arg,argnum,&arg);
    

    for(int i=0;i<filenum;i++){    
        if(isDir(NameList[i])){
            followTrace(NameList[i],arg);
        }
        else{
            struct stat buf;
            if(stat(NameList[i],&buf)==-1){
                perror("stat");
                exit(EXIT_FAILURE);
            }
            printFile(NameList[i],arg);  
         }
    }
     
    
    
}