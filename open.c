#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
int main()
{
    int fd=open("open测试.txt",O_RDONLY|O_CREAT|O_TRUNC,0644);
    close(fd);
}
/*
O_TRUNC清0
此open若存在则清0，若不存在则创建，与指定权限0644
所以啊这个只读被清0你当然什么都看不到了

*/