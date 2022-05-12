/*
Copyright (C) 2018 SunChen
*/
#ifndef __COMMON_H
#define __COMMON_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>/*待选*/
#include <errno.h>
#include <fcntl.h>/*for nonblocking*/
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>/*for TCP_XXX defines*/

#define SA struct sockaddr
#define MAXSIZE 128
#define SERV_PORT 9877

#define LISTENQ 5

#define Serv_MAX_EVENTS 1024
#define Client_MAX_EVENTS 2

void setnonblocking(int fd);
void Setsockopt(int sockfd,int level,int optname,const void *optval,socklen_t optlen);
void sock_ntop(const SA *soaddr,char *straddr);
void Listen(int fd,int backlog);
int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Accept(int lifd,SA * cliaddr,socklen_t * clilen);
int Write(int fd,const char * buf,size_t size);
int Read(int fd,char* buf,size_t size);
int Epoll_create(int size);
void Epoll_ctl(int epfd,int op,int fd,struct epoll_event *ev);
int Epoll_wait(int epfd,struct epoll_event *events,int maxevent,int timeout);

void str_cli(FILE* fd,int sockfd);
int sock_pton(const char *strIP,SA *soaddr);


#endif
