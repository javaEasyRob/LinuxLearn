#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
int main(int argc,char**argv)
{
    DIR*dp;
    struct dirent *sdp;
    dp=opendir(argv[1]);
    while (sdp=readdir(dp))
    {
        printf("%s\t",sdp->d_name);
    }
    printf("\n");
    closedir(dp);

}