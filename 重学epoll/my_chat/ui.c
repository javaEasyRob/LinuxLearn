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
        int ret=scanf("%d",&ch);
        while(getchar()!='\n')
            continue;
        if(ret!=1||ch>'3'||ch<='0'){
            puts("输入错误,重新输入");
            continue;
        }
    }while(1);
    return ch;
}