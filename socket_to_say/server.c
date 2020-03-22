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
#include<time.h>
#define END_FLAG "##"
#define NAMESIZE 10
int main()
{
    srand(time(0));
    char buf[BUFSIZ];
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    int opt;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEPORT,(void*)&opt,sizeof(opt));
    struct sockaddr_in addr,client_addr;
    addr.sin_port=htons(8888);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(sfd,128);
    int len=sizeof(client_addr);
    char clientip[BUFSIZ];
    int cfd;
    while(( cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
        continue;
        printf("cfd=%d\n",cfd);
    printf("client ip:%s     port :%d\n",
    inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
    ntohs(client_addr.sin_port));//client port is admin by system
    int getlen;
    while(1){
        int choice=0;
        puts("1>接受");
        scanf("%d",&choice);
        while(getchar()!='\n')
            continue;
        if(choice==1)  
        {
            char fileName[256];
            // sprintf(fileName,"sad""%d"".png",rand()%65536+1);
            sprintf(fileName,"sad""%d"".mkv",rand()%65536+1);
            unlink(fileName);
            int fd=open(fileName,O_CREAT|O_TRUNC|O_RDWR,0644);
            if(fd==-1){
                perror("open");
            }
            while((getlen=read(cfd,buf,sizeof(buf)))!=0){
                printf("%d\n",getlen);
                if(getlen==-1&&errno==EINTR){
                  puts("客户端退出");
                    break;
                }else if(getlen>0){
                    int end_flag_len=sizeof(END_FLAG);
                    if((getlen==end_flag_len)&&(strcmp(buf,END_FLAG)==0)){
                            puts("文件读取结束了");
                            break;
                    }
                    write(fd,buf,getlen);
                    char str[]="传输中";
                    printf("Server:%s\n",str);
                    write(cfd,str,sizeof(str));
                }else{
                    perror("some thing wrong");
                    exit(EXIT_FAILURE);                
                }
            }
             if(getlen==0){
                    puts("客户端退出");
                    close(cfd);
                    close(fd);
            }else{
                // perror("some thing wrong");   
                puts("传输成功啦!");
                // exit(EXIT_SUCCESS);

            }
        }                   
    }
    close(cfd);
    close(sfd);
}