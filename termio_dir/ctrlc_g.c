/*
本程序通过term.c_cc[VEOF]=(cc_t)0x10;
将此终端暂时性的ctrl+d--->ctrl+g
*/
#include <termio.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
int main(void){
//term用于存储获得的终端参数信息
     struct termios term,oldterm;
     int err;
     //获得标准输入的终端参数，将获得的信息保存在term变量中
     if(tcgetattr(STDIN_FILENO,&term)==-1){
          perror("Cannot get standard input description");
          return 1;
     }
     //修改获得的终端信息的结束控制字符
     oldterm=term;
     term.c_cc[VEOF]=(cc_t)0x07;
          //使用tcsetattr函数将修改后的终端参数设置到标准输入中
     //err用于保存函数调用后的结果
     err=tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);
     //TCSAFLUSH：清空输入输出缓冲区才改变属性。
     //如果err为-1或是出现EINTR错误（函数执行被信号中断），
     //给出相关出错信息
     if(err==-1 && errno==EINTR){
          perror("Failed to change EOF character");
          return 1;
     }
     if (tcsetattr(STDIN_FILENO,TCSAFLUSH, &oldterm) < 0) {//更改设置为最初的样子
     return -1;
     }
     while(1);
     return 0;
}
