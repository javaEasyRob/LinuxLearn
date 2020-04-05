#include"load_person_store.h"
// PersonInfo personInfoList[MAXPEO];
Person personList[MAXPEO];
int loadPerson(int*personNum,Person*peoList)
{
    int fd=open(SETTINGFILE,O_RDONLY);
    int i=0;
    while(1){
        int ret=read(fd,&peoList[i].infoList,sizeof(PersonInfo));
        if(ret==-1){perror("open");return -1;}
        else if(ret==0)break;
        else i++;
    }
    close(fd);
    personNum=i;
    return 0;
}
int addPerson(int*currentNum,Person*peo)
{
    int fd=open(SETTINGFILE,O_APPEND);
    int ret=write(fd,&peo,sizeof(PersonInfo));
    if(ret==-1){perror("write");return -1;}
    else *currentNum++;
    close(fd);
    return 0;
}
// Personload()

// Person personList[PEONUM]={{{"adl","adl"},-1},{{"hzn","hzn"},-1},{{"sad","sad"},-1}};
