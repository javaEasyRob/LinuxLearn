#include<stdio.h>
#include<stdlib.h>
int main()
{
    char buf[4];
    FILE*fp= fopen("fopen测试.txt","r+b");
    fwrite("asd",sizeof("asd"),1,fp);
    fseek(fp,0L,SEEK_SET);
    fread(buf,sizeof(buf),1,fp);
    printf("%s",buf);
    fclose(fp);
    fp= fopen("fopen测试.txt","r+b");
    char s[]="zzzzzzqzzzzzzzzzzzzzzzzzzzzz";
    fwrite(s,sizeof(s),1,fp);
    fclose(fp);
    
}