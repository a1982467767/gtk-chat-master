/*
Copyright (C) 2018 SunChen
*/

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

void setnonblocking(int fd);
void
Setsockopt(int sockfd,int level,int optname,const void *optval,socklen_t optlen);


void str_cli(void *argv);
int sock_pton(const char *strIP,SA *soaddr);


/*****************************************************************************************************/
/*!

 */
void setnonblocking(int fd)
{
    int flags;

    if((flags = fcntl(fd, F_GETFL,0)) < 0)
    {
        fprintf(stderr,"fcntl() F_GETFL error :%s\n", strerror(errno));
        exit(1);
    }

    flags |= O_NONBLOCK;

    if(fcntl(fd,F_SETFL,flags)<0)
    {
        fprintf(stderr,"fcntl() F_GETFL error :%s\n", strerror(errno));
        exit(1);
    }

}


/*****************************************************************************************************/
/*!

 */
void
Setsockopt(int sockfd,int level,int optname,const void * optval,socklen_t optlen){
    if(setsockopt(sockfd,level,optname,optval,optlen)!=0){

        fprintf(stderr,"Setsockopt error :%s\n",strerror(errno));
        exit(1);
    }
}


/*****************************************************************************************************/
/*!

 */
int Epoll_wait(int epfd,struct epoll_event *events,int maxevent,int timeout)
{
    int nfds;

    while((nfds=epoll_wait(epfd,events,maxevent,timeout))==-1){

        if(errno==EINTR)
            continue;
        else{
            fprintf(stderr,"epoll_wait() error :%s\n",strerror(errno));
            exit(1);
        }
    }

    return nfds;
}

/*****************************************************************************************************/
/*!

 */
void Epoll_ctl(int epfd,int op,int fd,struct epoll_event *ev){

    if(epoll_ctl(epfd,op,fd,ev)==-1){
        fprintf(stderr,"fd:%d   epoll_ctl() error:%s\n",fd,strerror(errno));
        exit(1);
    }
    return;
}

/*****************************************************************************************************/
/*!

 */
int Epoll_create(int size){

    int epfd;

    if((epfd=epoll_create(size))==-1){
        fprintf(stderr,"epoll_create() error:%s\n",strerror(errno));
        exit(1);
    }
    return epfd;
}


/*****************************************************************************************************/
/*!

 */
int Write(int fd,const char * buf,size_t size){

    int nleft,nwrite;
    const char *ptr;

    nleft=size;
    ptr=buf;

    while(nleft > 0){

        if((nwrite = write(fd, ptr, nleft)) <= 0)
        {
            if(errno == EINTR)
                nwrite = 0;
            if(errno == EAGAIN)/*针对ET*/{
                if(size == nleft)
                    return -1;
                else
                    break;
            }
            else{
                fprintf(stderr,"write() error:%s\n",strerror(errno));
                exit(1);
            }
        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    return (size-nleft);
}


/*****************************************************************************************************/
/*!

 */
int Read(int fd,char* buf,size_t size){

    int nread, nleft;
    char *ptr;

    ptr=buf;
    nleft=size;
    printf("a\n");
    while(nleft > 0){
        printf("b\n");
        if((nread = read(fd,ptr,nleft))<0){
            printf("c\n");
            if(errno == EINTR)
                nread = 0;
            if(errno == EAGAIN)/*针对ET*/{
                if(size == nleft)
                    return -1;
                else
                    break;
            }
            else{
                fprintf(stderr,"read() error:%s\n",strerror(errno));
                fflush(stdin);
                fflush(stderr);
                exit(1);
            }
        }
        else if(nread==0)
            break;
            printf("d\n");
        ptr += nread;
        nleft -= nread;
        if(*(ptr-1)=='\n')
            break;
    }
                printf("f\n");
    return (size-nleft);
}
int my_read(int fd,void *buffer,int length)
{
int bytes_left;
int bytes_read;
char *ptr;

bytes_left=length;
while(bytes_left>0)
{
    bytes_read=read(fd,ptr,bytes_read);
    if(bytes_read<0)
    {
      if(errno==EINTR)
         bytes_read=0;
      else
         return(-1);
    }
    else if(bytes_read==0)
        break;
     bytes_left-=bytes_read;
     ptr+=bytes_read;
}
return(length-bytes_left);
}
/*****************************************************************************************************/
/*!

 */
/*success return 1 ,failed return -1*/
int sock_pton(const char* strIP,SA *soaddr){

    int n;
    struct sockaddr_in *sin=(struct sockaddr_in *)soaddr;

    if((n=inet_pton(AF_INET,strIP,&sin->sin_addr))==0){
        perror("IP address is unavaliable!\n");
        return -1;
    }
    else if(n<0){
        fprintf(stderr,"inet_pton() error:%s\n",strerror(errno));
        return -1;
    }
    else
        return 1;

}


