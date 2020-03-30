#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<stdbool.h>
#include <sys/types.h>  
#include"../head.h"
#define MSG_EXIT 0
#define MSG_ACK 1
#define MSG_CNT  2
#define MSG_LOGIN 3
#define PWDSIZE 10
#define NAMESIZE 10
#define MAX_EVENTS  1024      //监听上限
#define SERV_PORT   8887
int g_efd;
typedef struct PACK{
    int msg_kind;
    char packSender[NAMESIZE];
    char packRecver[NAMESIZE];
    char buf[BUFSIZ];
}PACK;
//这个结构体最好只放事件的信息
struct myevent_s{
    int fd;
    uint32_t events;
    void*arg;
    void(*callback)(void*);
    bool status;//1在树上，0不在树上

}my_events[MAX_EVENTS+1];

typedef struct personinfo{
    char name[NAMESIZE];
    char passwd[PWDSIZE];
    // int cfd;
}PersonInfo;
typedef struct Name_fd_map{
    char name[NAMESIZE];
    int cfd;
}Name_fd_map;
PersonInfo infoList[3]={{"adl","adl"},{"hzn","hzn"},{"sad","sad"}};
Name_fd_map searchMap[3]={{"adl",-1},{"hzn",-1},{"sad",-1}};
void servlogin(void*arg);
void recvdata(void*arg);
void senddata(void*arg);
void eventset(struct myevent_s* mv,int fd,void(*callback)(void*),void*arg);
void eventadd(int efd,int event,struct myevent_s*ms);
void eventdel(int efd,struct  myevent_s*ms);
void acceptconn(void*arg);
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
    }else{
        op=EPOLL_CTL_MOD;
        ms->status=1;
    }
    printf("现在发生的[event=%d]\n",event);
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
    int clien_len=sizeof(clien_addr);
    int cfd=accept(listenfd,(struct sockaddr *)&clien_addr,&clien_len);
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
    int lfd=socket(AF_INET,SOCK_STREAM,0);
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
    bind(lfd,(struct sockaddr*)&sin,sizeof(sin));
    listen(lfd,20);
    return ;
}
void servlogin(void*ptr)
{
    struct myevent_s*arg=(struct myevent_s*)ptr;
   int cfd=arg->fd;
   PACK login_pack;
    recv(cfd,&login_pack,sizeof(login_pack),0);
    if(login_pack.msg_kind==MSG_LOGIN){
        int i=0;
        for( i=0;i<3;i++){
            if(0==strcmp(login_pack.packSender,infoList[i].name)&&
                strcmp(login_pack.buf,infoList[i].passwd)==0){
                strcpy(login_pack.buf,"登录成功");
                send(cfd,&login_pack,sizeof(login_pack),0);
                searchMap[i].cfd=cfd;
                eventdel(g_efd,arg);
                eventset(arg,cfd,recvdata,arg);
                eventadd(g_efd,EPOLLIN,arg);
                return;
            }
        }
        if(i==3){
                strcpy(login_pack.buf,"登录失败,重新输入");
                send(cfd,&login_pack,sizeof(login_pack),0);
                return;
        }
        
    }else if(login_pack.msg_kind==MSG_EXIT){
        eventdel(g_efd,arg);
        printf("[cfd=%d]客户退出",arg->fd);
        close(arg->fd);
        
        return ;
    }else{
        fprintf(strerror,"登录时发送来错误的类型");
        return;
    }
}

void recv_send_data(void*ptr)
{
    struct myevent_s*arg=(struct myevent_s*)ptr;
    int cfd=arg->fd;
    PACK c_pack;
    int ret=recv(cfd,&c_pack,sizeof(c_pack),0);
    if(ret==-1){
        close(cfd);
        printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
    }else if(ret==0){
        eventdel(g_efd,arg);
        close(cfd);
        printf("[cfd=%d]客户退出",cfd);
    }else{
        // printf("%s--->%s\n",c_pack.packSender,c_pack.packRecver);
        if(c_pack.msg_kind==MSG_CNT){
            int i;
            for(i=0;i<3;i++){
                if(strcmp(c_pack.packRecver,searchMap[i].name)==0){
                    if(searchMap[i].cfd==-1){
                        s_gets(c_pack.buf,"sorry,对方不在线啊");
                        send(cfd,&c_pack,sizeof(c_pack),0);
                    }else{
                        send(searchMap[i].cfd,&c_pack,sizeof(c_pack),0);

                    }
                }
            }
            if(i==3){
                s_gets(c_pack.buf,"sorry,你没这好友啊");
                send(cfd,&c_pack,sizeof(c_pack),0);
            }
        }else{
                s_gets(c_pack.buf,"sorry,暂时不处理啊");
                send(cfd,&c_pack,sizeof(c_pack),0);
        }
    }

}
void senddata(void*ptr)
{
    struct myevent_s*arg=(struct myevent_s*)ptr;
    int cfd=arg->fd;
    char buf[BUFSIZ];
    puts("server:");
    s_gets(buf,BUFSIZ);
    int ret=send(cfd,buf,BUFSIZ,0);
    eventdel(g_efd,arg);
    if(ret==-1){
        close(cfd);
        printf("recv[fd=%d] error[%d]:%s\n", cfd, errno, strerror(errno));
    }else if(ret==0){
        close(cfd);
        printf("[cfd=%d]客户退出",cfd);
    }else{
    eventset(arg,cfd,recvdata,arg);
    eventadd(g_efd,EPOLLIN,arg);
    }
}

int main(int argc,char**argv)
{
    
    unsigned short port=SERV_PORT;
    if(argc==2)
        port=atoi(argv[1]);
    struct epoll_event all_events[MAX_EVENTS+1];
    g_efd=epoll_create(MAX_EVENTS+1);
    puts("连接中....");
    initlistensocket(g_efd,port);
    while(1){
        int nfd=epoll_wait(g_efd,all_events,MAX_EVENTS+1,1000);
        if(nfd==0){
            puts("没有事就是好事");
        }
        for(int i=0;i<nfd;i++){
            struct myevent_s*ev=(struct myevent_s*)all_events[i].data.ptr;
            if((all_events[i].events&EPOLLIN)&&(ev->events&EPOLLIN)){
                ev->callback(ev->arg);
            }
        }
    }
    return 0;
}