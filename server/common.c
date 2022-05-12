/*
Copyright (C) 2018 SunChen
*/
#include "common.h"

/*****************************************************************************************************/
/*!

 */
void setnonblocking(int fd)
{

    int flags;

    if((flags=fcntl(fd,F_GETFL,0))<0){
        fprintf(stderr,"fcntl() F_GETFL error :%s\n",strerror(errno));
        exit(1);
    }

    flags |= O_NONBLOCK;

    if(fcntl(fd,F_SETFL,flags)<0){
        fprintf(stderr,"fcntl() F_GETFL error :%s\n",strerror(errno));
        exit(1);
    }

}

/*****************************************************************************************************/
/*!

 */
void Setsockopt(int sockfd,int level,int optname,const void *optval,socklen_t optlen)
{
    if(setsockopt(sockfd,level,optname,optval,optlen)!=0){

        fprintf(stderr,"Setsockopt()  error :%s\n",strerror(errno));
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
    const  char *ptr;

    nleft=size;
    ptr=buf;

    while(nleft > 0){

        if((nwrite=write(fd,ptr,nleft)) <= 0){
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

    while(nleft > 0){

        if((nread = read(fd,ptr,nleft))<0){
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

        ptr += nread;
        nleft -= nread;
        if(*(ptr-1)=='\n')
            break;
    }
    return (size-nleft);
}

/*****************************************************************************************************/
/*!

 */
void sock_ntop(const SA *soaddr,char *straddr)
{
    char buf[MAXSIZE],portstr[8];

    switch(soaddr->sa_family){

    case AF_INET:{

        struct sockaddr_in *sin=(struct sockaddr_in *)soaddr;
        if(inet_ntop(AF_INET,&sin->sin_addr,buf,MAXSIZE)==NULL){
            straddr[0]='\0';
            return;
        }
        if(ntohs(sin->sin_port)!=0){
            snprintf(portstr,sizeof(portstr),":%d",ntohs(sin->sin_port));
            strcat(buf,portstr);
        }
        strcpy(straddr,buf);
        return;
    }
#ifdef IPV6

    case AF_INET6:{

        struct sockaddr_in6 *sin6=(struct sockaddr_in6 *)soaddr;
        buf[0]='[';

        if(inet_ntop(AF_INET6,&sin6->sin6_addr,buf+1,MAXSIZE-1)==NULL){
            straddr[0]='\0';
            return;
        }
        if(ntohs(sin6->sin6_port)!=0){
            snprintf(portstr,sizeof(portstr),"]:%d",ntohs(sin6->sin6_port));
            strcat(buf,portstr);
            strcpy(straddr,buf);
        }
        else
            strcpy(straddr,buf+1);
        return;
    }
#endif
    }
}

/*****************************************************************************************************/
/*!

 */
int
Socket(int family, int type, int protocol)
{
    int             n;

    if((n=socket(family,type,protocol))<0){
        fprintf(stderr,"socket() error:%s\n",strerror(errno));
        exit(1);
    }
    return(n);
}

/*****************************************************************************************************/
/*!

 */
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if(bind(fd,sa,salen)<0){
        fprintf(stderr,"bind() error:%s\n",strerror(errno));
        exit(1);
    }
}

/*****************************************************************************************************/
/*!

 */
void Listen(int fd,int backlog){
    char *ptr;

    if((ptr=getenv("LISTENQ"))!=NULL)
        backlog=atoi(ptr);
    while(listen(fd,backlog)<0){
        if(errno==EINTR)
            continue;
        else{
            fprintf(stderr,"listen() error:%s\n",strerror(errno));
            exit(1);
        }
    }
}


/*****************************************************************************************************/
/*!
 * \brief Accept
 * \param lifd
 * \param cliaddr
 * \param clilen
 * \success return connfd,error return -1
 */
int Accept(int lifd,SA * cliaddr,socklen_t *clilen)
{
    int connfd;
    while((connfd=accept(lifd,cliaddr,clilen))<0)
    {
        if(errno==EINTR)
            continue;
#ifdef EPROTO
        else if(errno==EPROTO || errno==ECONNABORTED)
#else
        else if(errno==ECONNABORTED)
#endif
            continue;
        else{
            fprintf(stderr,"accept() error:%s\n",strerror(errno));
            exit(1);
        }
    }
    return connfd;
}

/*****************************************************************************************************/
/*!

 */
void str_cli(FILE * fp,int sockfd)
{
    char buf[MAXSIZE];
    int  n;
    int nfds, epfd;
    struct epoll_event ev, events[Client_MAX_EVENTS];
    int i;

    epfd = Epoll_create( Client_MAX_EVENTS );

    bzero( &ev, sizeof(struct epoll_event) );
    ev.data.fd = fileno(fp);
    ev.events = EPOLLIN;/*LT*/
    Epoll_ctl( epfd, EPOLL_CTL_ADD, fileno(fp), &ev );

    bzero( &ev, sizeof( struct epoll_event ) );
    ev.data.fd = sockfd;
    setnonblocking( sockfd );/**/
    ev.events = EPOLLIN|EPOLLET;/*ET*/
    Epoll_ctl( epfd, EPOLL_CTL_ADD, sockfd, &ev );
    for(;;)
    {

        nfds = Epoll_wait(epfd, events, Client_MAX_EVENTS, -1);

        for( i = 0; i < nfds; ++i )
        {

            if( events[i].data.fd == fileno(fp) )
            {
                //sendMassageWindows(sockfd, "./client51");
                n = Read(fileno(fp), buf, MAXSIZE);
                //printf ("shuru:%s",buf);
                if(n == 0 || strcmp(buf ,"exit\n") == 0)
                {
                    close(sockfd);
                    return;
                }
                else
                    Write( sockfd, buf, strlen(buf) );

                bzero(buf, strlen(buf));//test
                fflush(stdin);//test
            }
            if(events[i].data.fd==sockfd)
            {

                /*if n==-1 ,it said that read() return errno==EAGAIN*/
                while((n = Read(sockfd, buf, MAXSIZE)) != -1)
                {

                    if(n == 0)
                    {
                        perror("connect reset!\n");
                        close(sockfd);
                        return;
                    }
                    Write(fileno(stderr), buf, strlen(buf));
                    bzero(buf, strlen(buf));//test
                }
            }
        }
    }

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


