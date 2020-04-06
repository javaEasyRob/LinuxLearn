#include"c_io.h"
#include"head.h"

extern int cfd;
char GlobleName[NAMESIZE];
void return_exit()
{
    PACK exitPack;
    if(exitRequest(&exitPack,cfd)){exit(EXIT_SUCCESS);}
    else return;
}
void welcome()
{
    pthread_t pid1,pid2;
    
    pthread_create(&pid1,NULL,toRead,NULL);
    pthread_create(&pid2,NULL,toWrite,NULL);
    pthread_detach(pid1);
    pthread_detach(pid2);
    while(1);
    // pthread_join(pid1,NULL);
    // pthread_join(pid2,NULL);
    close(cfd);
}
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
void*toRead(void*arg)
{
    PACK recv_pack;
    while(1){
        readPack(&recv_pack);
    }
}
void*toWrite(void*arg)
{
    while(1){
        puts("1---私聊\t2---群发");
        int ch;
        int ret=scanf("%d",&ch);
        while(getchar()!='\n');
        if(ret!=1||ch<1||ch>2){
            puts("输入错误,请重新输入");
            continue;
        }
        switch (ch){
            case '1':
                privateChat();            
                break;
            case '2':
                groupChat();
            default:
                puts("出错了");
                break;
        }
    } 
    pthread_exit(NULL);
}
int logon(){}
int login()
{
    while(1){
        int ans=loginConfirmation();
        if(ans==0)welcome();
        if(ans==-2){
            puts("发来了错误类型!");
            puts("bye");
            exit(EXIT_FAILURE);
        }else if(ans==-1){
            if(exitRequest()==0){
                puts("bye");
                exit(EXIT_SUCCESS);
            }else continue;
        }
    }
}

void sendPack(PACK *pack)
{
    int sendlen=Send(cfd,pack,sizeof(*pack),0);
    if(sendlen==-1){
        close(cfd);
        printf("send[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }else{
        return;
    } 
}
void readPack(PACK *pack)
{
    int ret=Recv(cfd,pack,sizeof(*pack),0);
        if(ret==0){
            close(cfd);
            puts("服务器挂了");
            exit(EXIT_FAILURE);
        }else if(ret==-1){
            close(cfd);
            printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }else{    
            if(pack->msg_kind==MSG_CNT||pack->msg_kind==MSG_BROADCAST){ 
                printf("%s:%s\n",pack->packSender,pack->buf);
            }else if(pack->msg_kind==MSG_FAIL||pack->msg_kind==MSG_ACK)
                printf("%s\n",pack->buf);
        }
}
void privateChat()
{  
    PACK send_Pack;
    strcpy(send_Pack.packSender,GlobleName);
    send_Pack.msg_kind=MSG_CNT;
    printf("发送给:");
    s_gets(send_Pack.packRecver,NAMESIZE);//后续有列表了用数字
    printf("[输入框]%s:",GlobleName);        
    s_gets(send_Pack.buf,BUFSIZ);
    sendPack(&send_Pack);
}
void groupChat()
{   
    PACK send_Pack;
    strcpy(send_Pack.packSender,GlobleName);
    send_Pack.msg_kind=MSG_BROADCAST;
    printf("[输入框]%s:",GlobleName);        
    s_gets(send_Pack.buf,BUFSIZ);
    sendPack(&send_Pack);
}
int loginConfirmation()
{
    PACK login_Pack,ret_Pack;
    login_Pack.msg_kind=MSG_LOGIN;
    printf("姓名:");
    s_gets(login_Pack.packSender,NAMESIZE);
    printf("密码:");
    s_gets(login_Pack.buf,PWDSIZE);
    sendPack(&login_Pack);
    readPack(&ret_Pack);     
    if(ret_Pack.msg_kind==MSG_FAIL)return -1;
    if(ret_Pack.msg_kind==MSG_ACK){
        strcpy(GlobleName,login_Pack.packSender);
        return 0;    
    }
    else return -2;
}
int exitRequest()
{   
    PACK exit_Pack;
    puts("你要退出吗?");
    char ch= toupper(getchar());
    while(getchar()!='\n')
        continue;
    if(ch=='Y'){
        exit_Pack.msg_kind=MSG_EXIT;
        sendPack(&exit_Pack);
        close(cfd);
        return 0;
    }else return 1;
}