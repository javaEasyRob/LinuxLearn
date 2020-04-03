#ifndef HEAD_H
#define HEAD_H
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
char*s_gets(char*s,int n);
void sys_err(const char*str);
int Socket(int domain,int type,int protocol);
int Listen(int s, int backlog);
int Accept(int fd,struct sockaddr*sa,socklen_t *salenptr);
int Close(int fd);
int Bind(int  sockfd,  struct  sockaddr   *my_addr,socklen_t addrlen);
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
ssize_t Read(int fd,void*ptr,size_t nbytes);
int readn(int fd,void*vptr,size_t n);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf,size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);
ssize_t Send(int sockfd, void *buf,size_t len,int flags);
ssize_t Recv(int sockfd, void *buf,size_t len,int flags);

#endif