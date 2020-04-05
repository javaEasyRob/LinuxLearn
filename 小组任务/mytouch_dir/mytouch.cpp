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
#include <netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<dirent.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<errno.h>
#include<fcntl.h>
#include<iostream>
#include<string>
#include<fstream>
// #include"head.h"
using namespace std;
#define CONFIGTXT "/home/adl/桌面/linux/小组任务/mytouch_dir/mytouch.txt"
void mytouch(char*writeFileName,string&configFileName)
{
    string line;
    struct stat statbuf;
    if(lstat(writeFileName,&statbuf)==-1){
            ifstream is(configFileName);
            if(!is){cerr<<"出错了-----没有配置"<<endl;is.close();exit(1);}
            ofstream os(writeFileName,ofstream::out|ofstream::trunc);
            while(getline(is,line)){
                os<<line<<endl;
            }
            os.close();
        }else{
            puts("文件已经存在,先不操作"); 
            exit(1);
        }
}
int main(int argc,char**argv)
{
    string configtxt(CONFIGTXT);
    if(argc==1){
        puts("参数少！");
        exit(1);
    }else if(argc==2){
        mytouch(argv[1],configtxt);
    }else if(argc==3){
        configtxt=argv[2];
        mytouch(argv[1],configtxt);
    }
    return 0;   
}