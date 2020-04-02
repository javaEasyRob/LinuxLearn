#include"c_io.h"
#include"head.h"

int connect_init()
{
    int cfd=Socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
    puts("connecting...");
    while((Connect(cfd,(struct sockaddr*)&addr,sizeof(addr)))==-1);
    puts("connect correct\n");
    return cfd ;
}

void sendPack(int cfd,PACK *pack)
{
    if(Send(cfd,pack,sizeof(*pack),0)==-1){
        puts("粗错了");
        close(cfd);
        printf("send[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
        // pthread_exit(NULL);
        exit(EXIT_FAILURE);
    }   
}
void readPack(int cfd,PACK *pack)
{
    int ret=Recv(cfd,pack,sizeof(*pack),0);
        if(ret==0){
            close(cfd);
            puts("服务器挂了");
            exit(EXIT_FAILURE);
        }else if(ret==-1){
            puts("粗错了");
            close(cfd);
            printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }else{    
            if(pack->msg_kind==MSG_CNT||pack->msg_kind==MSG_BROADCAST){ 
                printf("%s:%s\n",pack->packSender,pack->buf);
            }
        }
}
void privateChat(PACK*send_Pack,int cfd,char*packSenderName)
{
    send_Pack->msg_kind=MSG_CNT;
    printf("发送给:");
    s_gets(send_Pack->packRecver,NAMESIZE);//后续有列表了用数字
    printf("[输入框]%s:",packSenderName);        
    s_gets(send_Pack->buf,BUFSIZ);
    sendPack(cfd,send_Pack);
}
void groupChat(PACK*send_Pack,int cfd,char*packSenderName)
{
    send_Pack->msg_kind=MSG_BROADCAST;
    printf("[输入框]%s:",packSenderName);        
    s_gets(send_Pack->buf,BUFSIZ);
    sendPack(cfd,send_Pack);
}
void loginConfirmation(PACK*login_Pack,PACK*ret_Pack,int cfd)
{
    login_Pack->msg_kind=MSG_LOGIN;
    printf("姓名:");
    s_gets(login_Pack->packSender,NAMESIZE);
    printf("密码:");
    s_gets(login_Pack->buf,PWDSIZE);
    sendPack(cfd,login_Pack);
    readPack(cfd,ret_Pack);         
}
int exitRequest(PACK*exit_Pack,int cfd,char*packSenderName)
{
    puts("你要退出吗?");
    char choice[5];
    s_gets(choice,5);
    if(!strcmp(choice,EXIT)||!strcmp(choice,"quit")){         
        exit_Pack->msg_kind=MSG_EXIT;
        strcpy(exit_Pack->packSender,packSenderName);
        sendPack(cfd,exit_Pack);
        close(cfd);
        return 1;
    }
    return 0;
}