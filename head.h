#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
#include<strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/mman.h>
#include<pthread.h>
#include<signal.h>
#include<semaphore.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<wait.h>

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
void sys_err(const char*str)
{
    perror(str);
    exit(-1);
}
int Socket(int domain,int type,int protocol)
{
    int n=socket(AF_INET,SOCK_STREAM,0);
    if(n==-1){
        sys_err(__func__);
        // return -1;
    }
    return n;
}
 int Listen(int s, int backlog)
 {
     int n;
     n=listen(s,backlog);
     if(n==-1){
         sys_err(__func__);
         return n;
     }
     return 0;
 }
 int Accept(int fd,struct sockaddr*sa,socklen_t *salenptr)
 {
    int n;
     while((n=accept(fd,sa,salenptr))==-1){
        if((errno==EINTR)||(errno==ECONNABORTED))
            continue;
        else{
            sys_err(__func__);
        }
     }
     return n;
}
int Close(int fd)
{
    int n;
    if((n=close(fd))==-1)
        sys_err(__func__);
    return n;
}
int Bind(int  sockfd,  struct  sockaddr   *my_addr,socklen_t addrlen)
{
    int n;
    if((n=bind(sockfd,my_addr,addrlen))<0){
        sys_err(__func__);
    }
    return n;
}
 

int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
 {
     int n=connect(sockfd,addr,addrlen);
    if(n<0){
        // sys_err(__func__);
        return n;
    }
    return n;
}
ssize_t Read(int fd,void*ptr,size_t nbytes)
{
    ssize_t n;
    while((n=read(fd,ptr,nbytes))==-1){
        if(errno==EINTR)
            continue;   
        else
            return -1;
    }
    // printf("%ld\n",n);
    return n;
}
int readn(int fd,void*vptr,size_t n)
{
      size_t          nleft = n;          //readn函数还需要读的字节数
    ssize_t        nread = 0;          //read函数读到的字节数
    unsigned char  *ptr = (unsigned char *)vptr; //指向缓冲区的指针

    while (nleft > 0)
    {
        nread = read(fd, ptr, nleft);
        if (-1 == nread)
        {
            if (EINTR == errno)//对端退出
                nread = 0;
            else
                return -1;
        }
        else if (0 == nread)//对端退出
        {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;
}
ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft=count;//剩余字节数
	ssize_t nwritten;//已经写入的字节数
	char *bufp = (char*)buf;//

	while(nleft>0)
	{
		if((nwritten=write(fd, bufp, nleft))<0)
		{
			if(errno==EINTR)//信号中断
			{
				continue;
			}
			return -1;//否则出错
		}
		else if(nwritten==0)
		{
			continue;
		}
		bufp+=nwritten;//进行指针偏移
		nleft -= nwritten;
	} 
	return count;
}
ssize_t recv_peek(int sockfd, void *buf,size_t len)
{
	while(1)
	{
		//recv函数只用于套接口
		//recv函数读取后，不将数据在缓冲区清除
		int ret= recv(sockfd, buf, len, MSG_PEEK);
		if(ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}
ssize_t Recv(int sockfd, void *buf,size_t len,int flags)
{
	while(1)
	{
		//recv函数只用于套接口
		//recv函数读取后，不将数据在缓冲区清除
		int ret= recv(sockfd, buf, len,flags);
		if(ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}
ssize_t Send(int sockfd, void *buf,size_t len,int flags)
{
	while(1)
	{
		//recv函数只用于套接口
		//recv函数读取后，不将数据在缓冲区清除
		int ret= send(sockfd, buf, len,flags);
		if(ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret;//设置窥探返回值
	int nread;//设置已窥探字符数
	char *bufp = (char*)buf;//缓存buf
	int nleft = maxline;//设置maxline为包最大长度，nleft为剩余需读取字符数
	while(1)
	{
		ret=recv_peek(sockfd, bufp, nleft);
		if(ret<0)
		{
			return ret;
		}
		else if(ret==0)
		{
			return ret;//对方终止了传送
		}

		nread=ret;
		int i;//检测有没有‘\n’字符，有则读取
		for(i=0; i<nread;i++)
		{
			if(bufp[i]=='\n')
			{
				ret= readn(sockfd,bufp, i+1);
				if(ret != i+1)//已经窥探到有i+1字符，如果没有则错误
					exit(EXIT_FAILURE);
				return ret;
			}
		}
		//如果没有读到‘\n’，则将消息读入，直到最大包
		nleft -= nread;
		ret= readn(sockfd, bufp, nread);
		if(ret != nread)//已经窥探到有nread个字符，如果readn函数不能读取这么多，则错误
		{
			exit(EXIT_FAILURE);
		}
		bufp += nread;
	}
	return -1;
}
