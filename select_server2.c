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
// #include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
#define NAMESIZE 10
typedef struct stu{
    int id;
    char name[NAMESIZE];
}Stu;

int main()
{
    Stu stu;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr,client_addr;
    addr.sin_port=htons(8888);
    addr.sin_family=AF_INET;
    
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(sfd,128);
    int maxfd=sfd;
    fd_set temp,reads;
    FD_ZERO(&reads);
    FD_SET(sfd,&reads);//这就是为什么之后
    int arr[FD_SETSIZE];//针对客户数量的数组
    memset(arr,-1,sizeof(arr));
    int msize=-1;
    while(1)
    {
        int ret;
        temp=reads;
        puts("select:");
        while((ret=select(maxfd+1,&temp,NULL,NULL,NULL))<0&&errno==EINTR)//阻塞,既处理read也处理accept//用户不在这里退出
            continue;
        
        // ret=select(maxfd+1,&temp,NULL,NULL,NULL);
        // printf("arr[0]=%d\n",arr[0]);
            // {printf("???\n");   continue;}
        //     perror("!!!");
        // printf("%s\n",strerror(errno));
        // printf("ret=%d\n",ret);
        if(ret==-1){
            perror("something wrong");
        }
        int len=sizeof(client_addr);
        if(FD_ISSET(sfd,&temp)){   
            int j;
            puts("accept someone");
            int cfd;
            while((cfd=accept(sfd,(struct sockaddr*)&client_addr,&len))==-1&&errno==EINTR)
                continue;
                // {printf("<<<\n"); continue;}
        //         perror("!!!");
        // printf("%s\n",strerror(errno));
            for ( j = 0; j < FD_SETSIZE; j++){
                if(arr[j]==-1){
                    arr[j]=cfd;
                    break;
                }
            }
            if(j==FD_SETSIZE){
                fputs("too many client\n",stderr);
            }
            FD_SET(cfd,&reads);
            maxfd=cfd>maxfd?cfd:maxfd;
            msize=msize>j?msize:j;
            if(ret==1)//ret==1是指select过滤只有客户端来连接，没有客户端读入，那么continue就可以了;
                continue;
        }
            
            
            
        for (int i =0; i <=msize; i++)
        {
            if(arr[i]!=-1&&FD_ISSET(arr[i],&temp))  //注意这里这样的话前面的cfd得下一轮循环才能去读入,此时cfd并不在temp集合中
            {
                int readlen;
                while((readlen=read(arr[i],&stu,sizeof(stu)))==-1&&EINTR==errno)
                    continue;
                    // {printf("!!!\n");continue;}
                    // perror("???");
                            // printf("%s\n",strerror(errno));

                if(readlen==0)//用户在这里退出
                {
                    puts("客户退出");
                    close(arr[i]);
                    FD_CLR(arr[i],&reads);
                    arr[i]=-1;
                    continue;
                }
                char clientip[BUFSIZ];
                printf("Client:  ip:%s\tport :%d\t",
                inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientip,sizeof(clientip)),
                ntohs(client_addr.sin_port));
                printf("Stu id=%d\tname=%s\n",stu.id,stu.name);
                char str[]="ok";
                printf("Server:%s\n",str);
                write(arr[i],str,sizeof(str));
                sleep(1);
            }
            if(ret==1)
                break; 
        }
    }        
    close(sfd);
}