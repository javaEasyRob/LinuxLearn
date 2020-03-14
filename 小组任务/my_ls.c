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
#include<errno.h>
#include<stdbool.h>
#include<signal.h>
#include<fcntl.h>
#include<math.h>
#define FILENAMESIZE 2048
#define FILEMAXNUM 65536
#define BLOCKSIZE 1024
#define ARGLEN 16
#define ARGNUM 16
#define ARG_NONE 0
#define ARG_A_SML   1
#define ARG_A_BIG  2
#define ARG_L   4
#define ARG_R_BIG   8
#define ARG_I   16
#define ARG_R_SML 32
#define ARG_S 64
#define ARG_T 128
int cmp ( char *a , char *b ,int mode);
void swap(char*a,char*b);
int randPartition(char* A[],int left, int right,int mode);
void quicksort(char* A[], int left, int right,int mode);
bool Lstat(char*fileName,struct stat *pbuf);
char*getLast(char *name);
bool isDir(char*fileName);
void analysisArg(char(*arg)[ARGNUM],int argcount,int *mode);
void analysisFileStat(char*fileName,int arg);
void followTrace(char*dirName,int arg);
void printFile(char*name,int arg);
void transfer(char**fileNameList,int filenum,int arg);

int cmp ( char *a , char *b ,int mode) 
{ 
    if(mode&ARG_T){
        struct stat abuf,bbuf;
        if(Lstat(a,&abuf)&&Lstat(b,&bbuf)){
            if(abuf.st_mtime!=bbuf.st_mtime){
                // if(strcmp(a,"a.cpp")==0&&strcmp(b,"b.c")==0){
                //     printf("%lu",bbuf.st_mtime-abuf.st_mtime);
                // }
                if(bbuf.st_mtime<abuf.st_mtime){
                    return 1;
                }else{
                    return -1;
                }
            }
        }
    }
    if(mode&ARG_R_SML)
        return strcmp(a,b); 
    else
        return strcmp(b,a);
} 
void swap(char*a,char*b)
{
    char temp[FILENAMESIZE];
    // printf("%s<-->%s\n",a,b);
    strcpy(temp,a);
    strcpy(a,b);
    strcpy(b,temp);
}
int randPartition(char* A[],int left, int right,int mode)
{
    //生成[left,right]内的随机主元
    int p = (round(1.0*rand()/RAND_MAX)*(right-left)+left);
    swap(A[p],A[left]);
    char temp[FILENAMESIZE];
    strcpy(temp,A[left]);
    while(left < right)
    {
        while(left < right && cmp(A[right],temp,mode)<0) right--;//反复左移
        strcpy(A[left] ,A[right]);
        while(left < right &&cmp( A[left] ,temp,mode)>0) left++;//反复右移
        strcpy(A[right] , A[left]);
    }
    strcpy(A[left] ,temp);
    return left;
}
void quicksort(char* A[], int left, int right,int mode)
{
    if(left < right) // 当前区间的长度超过1
    {
        //将[left,right]按A[right]一分为2
        int pos = randPartition(A,left,right,mode);
        quicksort(A, left, pos-1,mode);  //对左子区间递归进行快速排序
        quicksort(A, pos+1, right,mode); //对右子区间递归进行快速排序
    }
}
bool Lstat(char*fileName,struct stat *pbuf)
{
    if(lstat(fileName,pbuf)==-1){
        perror("不存在此文件夹或者出错了");
        // sleep(5);
        return false;
    }
    return true;
}
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
    if(!Lstat(fileName,&buf)){
        // perror("不存在此文件或者出了错");
        return false;
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
            if(arg[i][j]=='-'){
                continue;
            }else if(arg[i][j]=='a'){
                *mode|=ARG_A_SML;
            }else if(arg[i][j]=='A'){
                *mode|=ARG_A_BIG;
            }else if(arg[i][j]=='l'){
                *mode|=ARG_L;
            }else if(arg[i][j]=='R'){
                *mode|=ARG_R_BIG;
            }else if(arg[i][j]=='i'){
                *mode|=ARG_I;
            }else if(arg[i][j]=='r'){
                *mode|=ARG_R_SML;
            }else if(arg[i][j]=='s'){
                *mode|=ARG_S;
            }else if(arg[i][j]=='t'){
                *mode|=ARG_T;
            }else{
                printf("出现了奇怪的参数%c\n",arg[i][j]);
            }
            
        }
    }
}
// ls -arg
void printFile(char*name,int arg)
{
    char* lastName=getLast(name);
    // if(((strncmp(".",lastName,1)==0)||(strncmp("..",la))&&(!(arg&ARG_A_SML)))){
    //     // printf("return\n");
    //     return;
    // }
    
    if((!(arg&ARG_A_SML))){
        if(!(arg&ARG_A_BIG)){
            if(lastName[0]=='.'){
                return;
            }
        }else{
            if(strcmp(lastName,".")==0||strcmp(lastName,"..")==0)
                return;
        }

    }
    if(arg&ARG_S){
        struct stat buf; 
        if(Lstat(name,&buf)){
            long vsize=buf.st_size/BLOCKSIZE;
            long size;
            if(vsize<4){
                size=4;
            }else{
                if(vsize%4==0&&vsize*BLOCKSIZE==buf.st_size){
                    size=vsize;
                }else{
                    if(vsize%4!=0)
                        size=vsize%4+vsize;
                    else
                        size=vsize%4+vsize+4;
                }
            }
        printf("%-4ld ",size);
        }
    }
    if(arg&ARG_I){
        struct stat buf;
        if(!Lstat(name,&buf)){
            // perror("不存在此文件夹或者出了错");
            return;
        }
        unsigned int inode=buf.st_ino;
        printf("%u ",inode);
        
    }
    if(arg&ARG_L){
        
        analysisFileStat(name,arg);
    }else{
        printf("%-10s\t",lastName);
    }
}
//ls-l
void analysisFileStat(char*fileName,int arg)
{
    struct stat buf;
    if(!Lstat(fileName,&buf)){
        // perror("不存在此文件夹或者出错了");
        return;
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
    printf("%2ld  ",buf.st_nlink);
    printf("%-8s",psd->pw_name);
    printf("%-8s",grp->gr_name);
    printf("%6ld",buf.st_size);

    char buf_time[32];
    if(!(arg&ARG_T))
        strcpy(buf_time,ctime(&buf.st_ctime));
    else
        strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time)-1]='\0'; //去除换行
    printf("    %s  ",buf_time);
        char* lastName=getLast(fileName);

    printf("%s\n",lastName);

}
//跟踪目录
void followTrace(char*dirName,int arg)
{
    // if(strncmp(dirName,"/proc",5)==0){
    //     printf("proc 目录先不管\n");
    //     // return;
    // }
    // struct stat buf;
    // if(lstat(dirName,&buf)==-1){
    //     perror("不存在此文件");
    //     exit(EXIT_FAILURE);
    // }
    // mode_t mode=buf.st_mode;
    
    // if(S_ISLNK(mode)){
    //     printf("链接文件夹不打开");
    //     return;
    // }
    printf("%s:\n",dirName);
    int len=strlen(dirName);
    if(dirName[len-1]!='/')
        strncat(dirName,"/",2);
    DIR*dp;
    int filenum=0;
    struct dirent *sdp;
    // char fileNameList[FILEMAXNUM][FILENAMESIZE];
    char **fileNameList=malloc(FILEMAXNUM*sizeof(char*));

    if((dp=opendir(dirName))==NULL){
        perror("不允许访问");
        return;
    }
    while (sdp=readdir(dp)){
            fileNameList[filenum]=malloc(FILENAMESIZE*sizeof(char));
            sprintf(fileNameList[filenum++],"%s%s",dirName,sdp->d_name);
    }
    closedir(dp);
    transfer(fileNameList,filenum,arg);
    for(int i=0;i<filenum;i++)
        free(fileNameList[i]);
    free(fileNameList);
}
//中转站
void transfer(char**fileNameList,int filenum,int arg)
{
    if(filenum==0)
        return;
    char temp[FILENAMESIZE];
    
    // for (int i = 0; i < filenum-1; i++) {
    //     for (int j = i+1; j < filenum; j++){
    //          if(cmp(fileNameList[i],fileNameList[j])>0){
    //             //  printf("%s交换%s\n",fileNameList[i],fileNameList[j]);
    //             strcpy(temp,fileNameList[i]);
    //             strcpy(fileNameList[i],fileNameList[j]);
    //             strcpy(fileNameList[j],temp);
    //         }
    //     }   
    // }
    // qsort(fileNameList,filenum,sizeof(char*),cmp);
    // for (int i = 0; i < filenum; i++)
    // {
    //     printf("%s\n",fileNameList[i]);
    // }
    
    quicksort(fileNameList,0,filenum-1,arg);
    // qsort(fileNameList,filenum,sizeof(fileNameList[0]),cmp);
    for(int i=0;i<filenum;i++){    
        printFile(fileNameList[i],arg);  
        // sleep(1);      
    }
    printf("\n");
    for(int i=0;i<filenum;i++){    
        if(arg&ARG_R_BIG){
             char *lastName=getLast(fileNameList[i]);
            if(isDir(fileNameList[i])&&strcmp(lastName,".")&&strcmp(lastName,"..")){
                // printf("即将跟踪%s\n",fileNameList[i]);
                followTrace(fileNameList[i],arg); 
            }
        }

    }
}

int main(int argc,char**argv)
{
    remove("wrongfilewithanswer");
    signal(SIGINT,SIG_IGN);
    // char fileNameList[FILEMAXNUM][FILENAMESIZE];
    char **fileNameList=malloc(FILEMAXNUM*sizeof(char*));
    char Arg[ARGNUM][ARGLEN];

    int argnum=0;
    int filenum=0;

    /*分离文件和参数*/
    /*默认是当前文件夹*/
    
        for (int i = 1; i < argc; i++){
            if(argv[i][0]!='-'){
                fileNameList[filenum]=malloc(FILENAMESIZE*sizeof(char));
                strncpy(fileNameList[filenum++],argv[i],FILENAMESIZE);
                }else{
                strncpy(Arg[argnum++],argv[i],ARGLEN);
            }
        }
        if(filenum==0){
            fileNameList[filenum]=malloc(FILENAMESIZE*sizeof(char));
            strcpy(fileNameList[filenum++],".");
        }

    /*分析参数*/
    int arg=0;
    analysisArg(Arg,argnum,&arg);
    

    for(int i=0;i<filenum;i++){    
        if(isDir(fileNameList[i])){
            followTrace(fileNameList[i],arg);
        }
        else{
            struct stat buf;
            if(Lstat(fileNameList[i],&buf)){
                printFile(fileNameList[i],arg);
            }  
         }
         printf("\n");
         free(fileNameList[i]);
    }
    free(fileNameList);
    
}