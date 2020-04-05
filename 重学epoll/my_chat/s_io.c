#include"s_io.h"
#include"load_person_store.h"
extern Person personList[MAXPEO];
int g_efd=-1;
// Person personList[PEONUM]={{{"adl","adl"},-1},{{"hzn","hzn"},-1},{{"sad","sad"},-1}};
struct myevent_s my_events[MAX_EVENTS+1];

int ServerRetPack(int cfd,PACK*pack,char*retbuf,int msg_kind)
{
    // strcpy(pack->packSender,"Server");
    pack->msg_kind=msg_kind;
    strcpy(pack->buf,retbuf);
    int ret=Send(cfd,pack,sizeof(*pack),0);
    return ret;
}
int ServerRecvPack(PACK*pack,struct myevent_s*ptr)
{
    int cfd=ptr->fd;
    int ret=-1;
    int ffd=-1;
    PACK retPack;
    ret=Recv(cfd,pack,sizeof(*pack),0);
    // printf("%d\n",ret);
    // printf("%s\n",pack->packSender);
    // printf("%s\n",pack->packRecver);
    if(ret==-1){
        return -1;
    }else if(ret==0){
        printf("[cfd=%d]客户端退出啦\n",cfd);
        close(cfd);
        return -2;
    }else{
        switch (pack->msg_kind){
            case MSG_LOGIN:{
                int ret2=checkAccount(pack->packSender,pack->buf);
                // printf("%d\n",ret2);
                if(ret2>=0){
                    int ret3=ServerRetPack(cfd,&retPack,"登录成功",MSG_ACK);
                    if(ret3==-1){
                        fprintf(stderr,"发送失败");
                        return -3;
                    }else{
                        personList[ret2].fd=cfd;
                        eventdel(g_efd,ptr);
                        eventset(ptr,cfd,recv_send_data,ptr);
                        eventadd(g_efd,EPOLLIN,ptr);
                        return 0;
                    }
                }else{
                        char retbuf[BUFSIZ];
                    if(ret2==-1)
                        strcpy(retbuf,"登录失败,姓名错误");
                    else if(ret2==-2)
                        strcpy(retbuf,"登录失败,密码错误");
                    else if(ret2==-3)
                        strcpy(retbuf,"登录失败,用户已经登录");
                    int ret3=ServerRetPack(cfd,&retPack,retbuf,MSG_FAIL);
                    if(ret3==-1){
                        fprintf(stderr,"发送失败");
                        return -3;
                    }
                }
            break;
            }
            case MSG_EXIT:{
                eventdel(g_efd,ptr);
                printf("[cfd=%d]客户退出",ptr->fd);
                close(ptr->fd);
                return -2;
            break;
            }
            case MSG_CNT:{
                int ret2=seekFriend(pack->packSender,pack->packRecver);
                if(ret2==-1){
                    int ret3=ServerRetPack(cfd,&retPack,"无此好友",MSG_FAIL);
                    if(ret3==-1){
                        fprintf(stderr,"发送失败");
                        return -3;
                    }
                }else if((ffd=personList[ret2].fd)==-1){
                    int ret3=ServerRetPack(cfd,&retPack,"好友不在线，暂时不提供离线消息服务",MSG_FAIL);
                    if(ret3==-1){
                        fprintf(stderr,"发送失败");
                        return -3;
                    } 
                 }else{
                     int ret4=ServerSendPack(ffd,pack);
                     if(ret4==-1){
                         return -4;
                     }
                 }
            break;
            }
            case MSG_BROADCAST:{
                int ret=broadCast(cfd,pack);
                printf("收到消息的人数:%d\n",ret);
            break;
            }
            default:{
                fprintf(stderr,"发送的PACK类型错误");
                int ret3=ServerRetPack(cfd,&retPack,"发送的PACK类型错误",MSG_FAIL);
                if(ret3==-1){
                    fprintf(stderr,"发送失败");
                    return -3;
                }
                break;
                }
        }
    }
    return 0;
}
void servlogin(void*arg)
{
    struct myevent_s*ptr=(struct myevent_s*)arg;
    PACK login_pack;

    // int cfd=ptr->fd;
    int ret=ServerRecvPack(&login_pack,ptr);
    if(ret==-1){perror("读取err");return;}
    else if(ret==-2){return;}
    else if(ret==-3){perror("回写err");return ;}
    // ret=Recv(cfd,&login_pack,sizeof(login_pack),0);
    // if(ret==0){
    //     printf("[cfd=%d]客户端退出啦\n",cfd);
    //     close(cfd);
    //     return;
    // }
    // if(login_pack.msg_kind==MSG_LOGIN){
    //     int pos=findPersonName(login_pack.packSender);
    //     if(pos==-1||strcmp(personList[pos].infoList.passwd,login_pack.buf)||personList[pos].fd>0){
    //         strcpy(ret_pack.buf,"登录失败,重新输入");
    //         ret_pack.msg_kind=MSG_FAIL;
    //         if(ServerSendPack(cfd,&ret_pack)==-1)
    //             return;
    //         return;
    //     }
    //     else{
    //         ret_pack.msg_kind=MSG_ACK;
    //         strcpy(ret_pack.buf,"登录成功");
    //         if(ServerSendPack(cfd,&ret_pack)==-1)
    //             return;
    //         personList[pos].fd=cfd;
    //         eventdel(g_efd,ptr);
    //         eventset(ptr,cfd,recv_send_data,ptr);
    //         eventadd(g_efd,EPOLLIN,ptr);
    //         return;
    //         }
    // }else if(login_pack.msg_kind==MSG_EXIT){
    //     eventdel(g_efd,ptr);
    //     printf("[cfd=%d]客户退出",ptr->fd);
    //     close(ptr->fd);
    //     return ;
    // }else{
    //     fprintf(stderr,"登录时发送的PACK类型错误");
    //     return;
    // }
}
int  ServerSendPack(int cfd,PACK *pack)
{
    int ret=Send(cfd,pack,sizeof(*pack),0);
    return ret;
    // if(ret==-1)return -1;
        // puts("粗错了");
        // // close(cfd);
        // printf("send[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
        // return -1;
    // }   
    // return 0;
}
void recv_send_data(void*ptr)
{
    struct myevent_s*arg=(struct myevent_s*)ptr;
    int cfd=arg->fd,reserver_fd=-1;
    PACK connct_pack,ret_pack;
    int ret=ServerRecvPack(&connct_pack,ptr);
    if(ret==-1){perror("读取err");return;}
    else if(ret==-2){return;}
    else if(ret==-3){perror("回写err");return;}
    else if(ret==-4){perror("发送err");return;}
    // int ret=Recv(cfd,&connct_pack,sizeof(connct_pack),0);
    // if(ret==-1){
    //     close(cfd);
    //     printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
    // }else if(ret==0){
    //     eventdel(g_efd,arg);
    //     close(cfd);
    //     printf("[cfd=%d]客户退出",cfd);
    // }else{
    //     // printf("%s--->%s\n",connct_pack.packSender,connct_pack.packRecver);
    //     if(connct_pack.msg_kind==MSG_CNT){
    //         // int i;
    //         // for(i=0;i<PEONUM;i++){
    //         //     if(strcmp(connct_pack.packRecver,searchMap[i].name)==0){
    //         int pos=findPersonName(connct_pack.packRecver);
    //         if(pos==-1){
    //             ret_pack.msg_kind=MSG_FAIL;
    //             strcpy(ret_pack.buf,"sorry,你没这好友啊");
    //            if(ServerSendPack(cfd,&ret_pack)==-1)
    //                return;
    //             return;
    //         }else if((reserver_fd=personList[pos].fd)==-1){
    //             strcpy(ret_pack.buf,"sorry,对方不在线啊");
    //             if(ServerSendPack(cfd,&ret_pack)==-1)
    //                 return;  
    //         }else{
    //             if(ServerSendPack(reserver_fd,&connct_pack)==-1)
    //                 return;  
    //             }
    //     }else if(connct_pack.msg_kind==MSG_BROADCAST){
    //         for(int j=0;j<PEONUM;j++){
    //             reserver_fd=personList[j].fd;
    //             if(reserver_fd==-1||reserver_fd==cfd)
    //                 continue;
    //             if(ServerSendPack(reserver_fd,&connct_pack)==-1)
    //                 continue;  

    //         }
    //     }else{
    //             strcpy(ret_pack.buf,"sorry,暂时不处理啊");
    //             Send(cfd,&ret_pack,sizeof(ret_pack),0);
    //     }
    // }
}
int broadCast(int cfd,PACK*pack)
{
    int sum=0;
    for(int j=0;j<PEONUM;j++){
        int  ffd=personList[j].fd;
        if(ffd==-1||ffd==cfd)
            continue;
        if(ServerSendPack(ffd,pack)==-1){
            fprintf(stderr,"发送失败[ffd=%d]",ffd);
            continue;  
        }else{
            sum++;
        }
    }
    return sum;
}

int seekFriend(char *myName,char *friendName)
{
    // personList
    int pos=findPersonName(friendName);
    return pos;
}

void senddata(void*arg)
{
    //暂时弃用
    struct myevent_s*ptr=(struct myevent_s*)arg;
    int cfd=ptr->fd;
    char buf[BUFSIZ];
    puts("server:");
    s_gets(buf,BUFSIZ);
    int ret=Send(cfd,buf,BUFSIZ,0);
    eventdel(g_efd,arg);
    if(ret==-1){
        close(cfd);
        printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
    }else if(ret==0){
        close(cfd);
        printf("[cfd=%d]客户退出",cfd);
    }else{
    eventset(ptr,cfd,recvdata,ptr);
    eventadd(g_efd,EPOLLIN,ptr);
    }
}
int findPersonName(const char*name)
{  
    
     int i=0;
    for(i=0;i<PEONUM;i++){
        if(strcmp(personList[i].infoList.name,name)==0){
            return i;
        }
    }
    if(i==PEONUM){
        return -1;
    }
}
void DealEventLoop(struct epoll_event* all_events)
{
    int nfd=epoll_wait(g_efd,all_events,MAX_EVENTS+1,1000);
    if(nfd==-1){
        fprintf(stderr,"epoll_wait");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<nfd;i++){
        struct myevent_s*ev=(struct myevent_s*)all_events[i].data.ptr;
        if((all_events[i].events&EPOLLIN)&&(ev->events&EPOLLIN)){
            ev->callback(ev->arg);
        }
    }   
}
//设置我们自创的结构体的属性，保留EPOLLIN|。。。的设置。这里设置arg为自己的指针
void eventset(struct myevent_s* mv,int fd,void(*callback)(void*),void*arg)
{
    mv->fd=fd;
    mv->callback=callback;
    mv->events=0;//event 需要在eventadd中根据节点在不在树上确定需要的是啥
    mv->arg=arg;
    mv->status=0;
}
//捆绑我们自创事件的结构体到epollevent的data.ptr中，
//设置ctl的模式mod/add，设置epollevent in/out
void eventadd(int efd,int event,struct myevent_s*ms)
{
    struct epoll_event epevent;/*设置的时候不需要放在全局或者参数里面 ，
    尽管看看貌似也可以，但要解偶吧，把上树的篇幅设的比较大，但也就
    知道了这个epevent 放在里面其实挺好的，复用性强*/
    int op;
    if(ms->status==0){
        op=EPOLL_CTL_ADD;
        ms->status=1;
    }else{
        op=EPOLL_CTL_MOD;
    }
    // printf("现在发生的[event=%d]\n",event);
    epevent.events=event;
    ms->events=event;
    epevent.data.ptr=(void*)ms;
    epoll_ctl(efd,op,ms->fd,&epevent);
}
void eventdel(int efd,struct  myevent_s*ms)
{
    ms->status=0;
    epoll_ctl(efd,EPOLL_CTL_DEL,ms->fd,NULL);
}
void acceptconn(void*arg)
{
    struct myevent_s*mv=(struct myevent_s*)arg;
    int listenfd=mv->fd;
    struct sockaddr_in clien_addr;
    socklen_t clien_len=sizeof(clien_addr);
    int cfd=Accept(listenfd,(struct sockaddr *)&clien_addr,&clien_len);
    int i;
    for(i=0;i<MAX_EVENTS;i++){
        if(my_events[i].status==0)
            break;
    }
    if(i==MAX_EVENTS){
        fprintf(stderr,"%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
        return;
    }
    fcntl(cfd,F_SETFL,O_NONBLOCK);
    eventset(my_events+i,cfd,servlogin,my_events+i);
    eventadd(g_efd,EPOLLIN,my_events+i);
    /*[time:%ld]*/
    printf("new connect [%s:%d], pos[%d]\n", 
        inet_ntoa(clien_addr.sin_addr), ntohs(clien_addr.sin_port), i);
   
}

void initlistensocket(int efd,short port)
{
    int lfd=Socket(AF_INET,SOCK_STREAM,0);
    fcntl(lfd,F_SETFL,O_NONBLOCK);
    int opt;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
    eventset(my_events+MAX_EVENTS,lfd,acceptconn,my_events+MAX_EVENTS);
    eventadd(efd,EPOLLIN,my_events+MAX_EVENTS);
    struct sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    Bind(lfd,(struct sockaddr*)&sin,sizeof(sin));
    Listen(lfd,20);
    return ;
}
void recvdata(void*arg)
{
    //暂时弃用
    struct myevent_s*ptr=(struct myevent_s*)arg;
    int cfd=ptr->fd;
    char buf[BUFSIZ];
    int ret=Recv(cfd,buf,BUFSIZ,0);
    eventdel(g_efd,ptr);
    if(ret==-1){
        close(cfd);
        printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
    }else if(ret==0){
        close(cfd);
        printf("[cfd=%d]客户退出",cfd);
    }else{
        printf("client:%s\n",buf);
        eventset(ptr,cfd,senddata,ptr);
        eventadd(g_efd,EPOLLOUT,ptr);
    }

}
int checkAccount(char*Name,char*Passwd)
{
    // login_pack.packSender
    //login_pack.buf
        int pos=findPersonName(Name);//0
        // printf("%d\n",pos);
        if(pos==-1){return -1;}
        if(strcmp(personList[pos].infoList.passwd,Passwd)!=0){return -2;}
        if(personList[pos].fd>0){return -3;}
        return pos;
}
