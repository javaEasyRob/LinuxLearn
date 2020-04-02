#include"s_io.h"
int g_efd=-1;
//设置我们自创的结构体的属性，保留EPOLLIN|。。。的设置。这里设置arg为自己的指针
void eventset(struct myevent_s* mv,int fd,void(*callback)(void*),void*arg)
{
    mv->fd=fd;
    mv->callback=callback;
    mv->events=0;//event 需要在eventadd中根据节点在不在树上确定需要的是啥
    mv->arg=arg;
    mv->status=0;
}
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