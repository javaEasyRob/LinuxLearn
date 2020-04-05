#include"ui.h"
#include"head.h"
#include<ctype.h>
int menu()
{
    char choice=-1;
    int ch=-1;
    do{
    puts("----------------------------------------");
    puts("1-----登录\t2------注册\n3-----退出");
    puts("----------------------------------------");
    choice=getchar();
    while(getchar()!='\n')
        continue;
        ch=atoi(choice);
    }while(ch<=3&&ch>0);
    return ch;
}