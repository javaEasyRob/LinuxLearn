#ifndef LOAD_PERSON_STORE_H
#define LOAD_PERSON_STORE_H
#include<stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<stdbool.h>
#include <sys/types.h>  
// #include"head.h"

#define PWDSIZE 10
#define NAMESIZE 10
#define MAXPEO 1024
#define SETTINGFILE "personstore"
typedef struct personinfo{
    char name[NAMESIZE];
    char passwd[PWDSIZE];
}PersonInfo;
typedef struct Person{
    PersonInfo infoList;
    int fd;
}Person;

int loadPerson(int*personNum,Person*peoList);
int addPerson(int*currentNum,Person*peo);

#endif