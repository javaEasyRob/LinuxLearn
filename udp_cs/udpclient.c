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
#include<string.h>

#define NAMESIZE 10
#define BUFSIZE 1024
char*s_gets(char*s,int n)
{   char*find;
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
typedef struct stu{
    int id;
    char name[NAMESIZE];
}Stu;
int main()
{
    Stu stu;
    char buf[BUFSIZE];
    int sockfd=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    // addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8887);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    
    // while((connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1){
    //     // fprintf(stderr,"connect err");
    //     // exit(1);
    //     printf("connecting ...\n");
    //     sleep(1);
    // }
    while(1){   
        // char name[NAMESIZE];
        printf("id:");
        scanf("%d",&stu.id);
        while (getchar()!='\n')
            continue;
        printf("name:");
        s_gets(stu.name,NAMESIZE);

        
        int addr_len=sizeof(addr);
        printf("Client:send a Stu\n");
        sendto(sockfd,&stu,sizeof(stu),0,(struct sockaddr*)&addr,addr_len);
        recvfrom(sockfd,&buf,sizeof(buf),0,(struct sockaddr*)&addr,&addr_len);
        printf("Server:%s\n",buf);
        sleep(1);
    }
    close(sockfd);

}