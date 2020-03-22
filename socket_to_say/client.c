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
#include<string.h>
#define END_FLAG "##"
#define NAMESIZE 10
char*s_gets(char*s,int n)
{   
    char*find;
    char*ret_val;
    ret_val=fgets(s,n,stdin);
    if(ret_val){
        find=strchr(s,'\n');
        if(find){
            *find='\0';
        }else{
            while(getchar()!='\n')
            continue;
        }
    }
    return ret_val;
}
int main()
{
    char buf[BUFSIZ],readbuf[BUFSIZ];
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8888);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    while((connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1){
        printf("connecting ...\n");
        sleep(1);
    }
    printf("cfd=%d\n",cfd);
    
    while(1){
        int choice=0;
        
        puts("1>发送 ");
        scanf("%d",&choice);
        while(getchar()!='\n')
            continue;
        if(choice==1)  
        {
                // int fd=open("../软链接1.png",O_RDONLY);
                int fd=open("/home/adl/2020-03-19 21-34-38.mkv",O_RDONLY);

                if(fd==-1){
                    perror("open err");
                        exit(EXIT_FAILURE);
                }
                int len=0;    
                while(1){
                    len=read(fd,buf,sizeof(buf));
                    if(len==0){
                        puts("读到文件末尾");
                        int writelen=0;
                        
                        while((writelen=write(cfd,END_FLAG,sizeof(END_FLAG)))==-1&&errno==EINTR){
                            puts("服务器出了点小擦错");
                            exit(1);
                        }
                        printf("writelen=%d\n",writelen);
                        close(fd);
                        // close(cfd);
                        break ;
                    }else if(len==-1&&errno==EINTR){
                        perror("服务器出了错误");
                        exit(1);
                    }else if(len>0){
                        printf("读取到%d字节\n",len);
                        int writelen=0;
                        while((writelen=write(cfd,buf,len))==-1&&errno==EINTR){
                            puts("服务器出了点小擦错");
                            exit(1);
                        }
                        if(read(cfd,readbuf,sizeof(readbuf)))
                            printf("server :%s\n",readbuf);
                            // sleep(1);
                    }else{
                        perror("其他错误?");
                        exit(EXIT_FAILURE);
                    }
            }   
        }
        sleep(1);
    }
}