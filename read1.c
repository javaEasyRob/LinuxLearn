#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
int main()
{      
    char buf[1024];
    int getsize;
    int fd=open("软链接.png",O_RDONLY);//argv[1]
    int fd2=open("read测试.png",O_RDWR|O_CREAT|O_TRUNC,0644);//argv[2]
    while ( getsize=read(fd,buf,sizeof(buf)))
    {
        write(fd2,buf,getsize);
    }
    close(fd2);
    close(fd);
}

/*
由此可知O_TRUNC的作用是需要写入一个需要清空的文件写进取，
但这里没有它也没关系
从java中学习可知这个buffer的作用是更加高效的读取，或者说减少读写次数。
系统调用的速度比fgetc慢。。。因为fgetc竟然偷偷加了4096 buf
从用户空间到内核空间的系统调用耗时
*/