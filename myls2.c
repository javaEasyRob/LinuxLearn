#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>

void isFile(char*name);
void read_dir(char*dir)
{
    char path[256];
    DIR* dp=opendir(dir);
    struct dirent *sdp;
    while(sdp=readdir(dp)){
        if(strcmp(sdp->d_name,".")&&strcmp(sdp->d_name,"..")){
            sprintf(path,"%s/%s",dir,sdp->d_name);
            // printf("%s\n",path);
            isFile(path);
        }
    }

    closedir(dp);   
    return;
}
void isFile(char*name)
{
    int ret=0;
    struct stat sb;
    ret=stat(name,&sb);
    if(ret==-1){
        perror("stat error");
        return;
    }
    if(S_ISDIR(sb.st_mode)){
        read_dir(name);
    }
    printf("%s\t%ld\n",name,sb.st_size);

}
int main(int argc,char**argv)
{   
    // DIR*dp;
    // struct dirent *sdp;
    // dp=opendir(argv[1]);
    // while (sdp=readdir(dp))
    // {
    //     printf("%s\t",sdp->d_name);
    // }
    // printf("\n");
    // closedir(dp);
    if(argc==1){
        isFile(".");
    }else{
        isFile(argv[1]);
    }
    return 0;
}