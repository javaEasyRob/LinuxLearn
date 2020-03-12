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

#define NAMESIZE 1024
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
// typedef struct stu{
//     int id;
//     char name[NAMESIZE];
// }Stu;
int main()
{
    // Stu stu;
    char buf[BUFSIZE];
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8887);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    
    while((connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1){
        // fprintf(stderr,"connect err");
        // exit(1);
        printf("connecting ...\n");
        sleep(1);
    }
    while(1){   
        char name[NAMESIZE];
        int id;
        printf("name:");
        // scanf("%s",name);
        // while (getchar()!='\n')
        //     continue;
        // printf("name:");
        s_gets(name,NAMESIZE);

        
        
        write(cfd,name,sizeof(name));
        printf("Client:send a name:%s\n",name);
        read(cfd,&buf,sizeof(buf));
        printf("Server:%s\n",buf);
        sleep(1);
    }
    close(cfd);

}