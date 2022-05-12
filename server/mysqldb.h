/*
Copyright (C) 2018 SunChen
*/
#ifndef __MYSQLDB_H
#define __MYSQLDB_H

#define SELECT_ID "select id from chatuser;"
#define SELECT_NAME "select name from chatuser;"
#define INSERT "insert into message(sender,reciver,mes,time) value(\"%s\",\"%s\",\"%s\",\"%s\");"
#define UPDATECHATUSER "update chatuser set ipaddr = \"%s\", port = \"%d\",online = 1 where id = \"%s\" and online = 0;"
#define UPDATECHATUSEROFF "update chatuser set online = 0 where ipaddr = \"%s\" and port = \"%d\";"
#define SELECT_NAME_BY_IP "select name from chatuser where ipaddr = \"%s\" and port = \"%d\";"
#define SELECT_PASSWD_BY_ID "select passwd from chatuser where id = %s;"
#define SELECT_NAME_BY_ID "select name from chatuser where id = %s;"
#define LOGIN "select name from chatuser where id = \"%s\" and passwd = \"%s\";"
#define ONLINE "select id from chatuser where online = 1;"
#define NOSENDMESSAGE "select sender, reciver, mes, time from message where reciver = \"%s\" and havesend = 0;"
#define UPDATEMSGHAVESEND "update message set havesend = 1 where sender = \"%s\" and reciver = \"%s\" and mes = \"%s\" and time = \"%s\";"
#define ISONLINE "select * from chatuser where id = \"%s\" and online = 1;"
#define OFFLINE "update chatuser set online = 0"

struct msgstrcut
{
    char sender[10];
    char reciver[10];
    char msg[100];
    char time[22];
};

int getUserId(char userid[10][10]);
int getOnlineUserId(char onlineuserid[10][10]);
int getUserName(char username[10][10]);
int getNoSendMessage(char userid[], struct msgstrcut result[10]);
int insertMessage(char sender[], char reciver[], char message[], char time[]);
int updataChatUser(char sender[], char ip[], int port);
int updataChatUser_offline(char ip[], int port);
int updataMessageToHavesend(char sender[], char reciver[], char msg[], char time[]);
char * getUserNameByIp(char ipaddr[], int port);
char * getPasswdById(const char * userid_text);
char * getNameById(const char * userid_text);
int Login(char id[], char passwd[], char ipaddr[], int port);
int isOnline(char id[]);
int offOnline();

#endif
