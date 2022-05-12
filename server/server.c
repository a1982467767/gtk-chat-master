/*
Copyright (C) 2018 SunChen
*/
#include "common.h"
#include "mysqldb.h"
#include <time.h>

static char userid[10][10] = {"\0"} ;
static char onlineuserid[10][10] = {"\0"} ;
static char username[10][16] = {"\0"} ;

void do_service(int conn,char *addr, int port);
void messageHandle(char *buf,char *addr, int port,int sockfd,char *sender);
void server_login(char *buf,char *loginid,char *passwd,char *sender,char *addr, int port,int sockfd);
void server_reciverMessage(char *buf,char *reciver,char *message,char *sender,char *addr, int port,int sockfd);
void server_reciverack(char *buf, char *sender);
void server_returnUserId( char *buf, char *loginid, int sockfd);

/*****************************************************************************************************/
/*!

 */
int main(int argc,char *argv[])
{
    signal(SIGCHLD, SIG_IGN);
    int listenfd; //被动套接字(文件描述符），即只可以accept, 监听套接字
    struct sockaddr_in peeraddr; //传出参数
    struct sockaddr_in servaddr;
    socklen_t peerlen = sizeof(peeraddr);
    //传入传出参数，必须有初始值
    int conn; // 已连接套接字(变为主动套接字，即可以主动connect)
    int on = 1;
    pid_t pid;
    getUserId(userid);//更新ID表
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    /* inet_aton("127.0.0.1", &servaddr.sin_addr); */

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on,sizeof(on));

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);
    offOnline();
    while (1)
    {
        conn = Accept(listenfd,  (struct sockaddr *)&peeraddr, &peerlen);//3次握手完成的序列

        printf("sockfd:%d client :ip=%s port=%d connected \n", conn, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        pid = fork();
        if (pid == -1)
            fprintf(stderr,"fork error :%s\n",strerror(errno));
        else if (pid == 0)
        { // 子进程
            close(listenfd);
            do_service(conn, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
            exit(EXIT_SUCCESS);
        }
        else
            close(conn); //父进程
    }
    return 0;
}

/*****************************************************************************************************/
/*!

 */
void do_service(int conn, char *addr, int port)
{
    int sockfd, n;
    char buf[MAXSIZE];
    sockfd = conn;
    char sender[10] = {"\0"};  //潜在bug

    while(1)
    {
        //printf("11\n");
        n = read(sockfd, buf, MAXSIZE); //获取用户端发来的信息，存放在buf中，后面只需要根据buf操作即可
        printf("get client message:%s\n",buf);
        if(n == 0 || strcmp(buf,"exit") == 0)
        {
            char uname[16] = {"\0"};
            strcpy(uname,getUserNameByIp(addr, port));//获取该用户的用户名

            if (updataChatUser_offline(addr, port))  //在数据库中设定该用户离线
            {
                printf("update chatuser( %s set online = 0 ) success!\n",uname);
            }
            else
            {
                printf("update chatuser( %s set online = 0 )failed!\n",uname);
            }

            close(sockfd);
            printf("sockfd :%d disconnected\n",sockfd);//test
            break;
        }
        else  //对获取到的信息进行处理
        {
            messageHandle(buf, addr, port, sockfd, sender);//非退出模式，调用服务器信息处理函数
        }
    }
}

/*****************************************************************************************************/
/*!

 */
void messageHandle(char *buf, char *addr, int port, int sockfd, char *sender)
{
    char reciver[10] = {"\0"};
    char message[100] = {"\0"};
    char loginid[10] = {"\0"};
    char passwd[18] = {"\0"};
    int flag = 0; //0 login ,1 sendmessage
    int x =0;
    while(buf[x] != '\0')
    {
        if (buf[x] == '/') //判断从客户端接收到的信息是否是登录信息
        {
            flag = 0;
            break;
        }
        else if (buf[x] == '~')
        {
            flag = 3;
            break;
        }
        else
            flag = 1;
        ++x;
    }
    if(strncmp(buf,"msg:",4) == 0) //接收信息回应
    {
        flag = 2;
    }

    printf("flag:%d\n",flag);
    if(flag == 0) //登录
    {
        server_login( buf, loginid, passwd, sender, addr, port, sockfd);
    }
    else if(flag == 1)  //接受发送信息模块
    {
        server_reciverMessage( buf, reciver ,message, sender, addr, port, sockfd);
    }
    else if(flag == 2)  //信息确认模块
    {
        server_reciverack( buf, sender);
    }
    else if(flag == 3)  //给客户端提供用户信息模块
    {
        server_returnUserId( buf, loginid, sockfd);
    }
}

/*****************************************************************************************************/
/*!

 */
void server_login(char *buf,char *loginid,char *passwd,char *sender,char *addr, int port,int sockfd)
{
    char * p;
    int i = 0, pid, flag; //flag 返回登录状态
    struct msgstrcut result[10];
    p = buf;
    while(p)//拆分登录信息：id/passwd
    {
        if (*p != '/')
        {
            loginid[i] = *p;
        }
        else
        {
            ++p;
            break;
        }
        ++i;
        ++p;
    }
    strcpy(passwd,p);
    strcpy(sender, loginid);
    printf("buf:%s id:%s passwd:%s\n",buf,loginid,passwd);
    flag = isOnline(loginid);  //在线返回1 不在线返回 0
    if(flag == 0)  //不在线状态，登录
    {
        flag = Login(loginid, passwd, addr, port);
    }
    else
    {
        flag = 0;  //在线状态，不重复登录
    }
    if( flag == 1 )
    {
        strcpy(buf,"success");
        printf ("login success\n");
        Write(sockfd, buf, 7);

        pid = fork();
        if (pid == -1)
            fprintf(stderr,"fork error :%s\n",strerror(errno));
        else if (pid == 0) //子进程执行信息发送模块
        {
            while(1)
            {
                int i = 0;
                memset(result,'\0',sizeof (struct msgstrcut ) * 10);

                getNoSendMessage(loginid, result); //是否有未接受的消息，一次最多存放１０条未发送的信息
                while(strlen(result[i].reciver) != 0)
                {
                    //设定发送格式　amsg:sender_id|time|msg\0
                    printf("result[%d]: sender:%s, msg:%s, time:%s.\n",i,result[i].sender,result[i].msg,result[i].time);
                    strcpy(buf,"amsg:");
                    strcat(buf, result[i].sender);
                    strcat(buf,"|");
                    strcat(buf,result[i].time);
                    strcat(buf,"|");
                    strcat(buf,result[i].msg);
                    buf[strlen(buf)] = '\0';
                    Write(sockfd, buf, strlen(buf) + 1);
                    printf("sender to client %s:%s\n", result[i].reciver, buf);
                    ++i;
                    sleep(1);
                }
                sleep(1);
            }
        }
        else //父进程返回上一级
        {
            return ;
        }
    }
    else if( flag == 0)
    {
        strcpy(buf,"sameid");
        printf ("login failed\n");
        Write(sockfd, buf, 6);
    }
    else
    {
        strcpy(buf,"failed");
        printf ("login failed\n");
        Write(sockfd, buf, 6);
    }
    bzero(buf, strlen(buf));//test

}

/*****************************************************************************************************/
/*!

 */
void server_reciverMessage(char *buf,char *reciver,char *message,char *sender,char *addr, int port,int sockfd)
{
    time_t now ;
    struct tm *tm_now ;
    char *p = buf;
    char timebuf[22];
    int i = 0;
    time(&now) ;
    tm_now = localtime(&now) ;
    sprintf(timebuf,"%4d-%2d-%2d %2d:%2d:%2d", \
            tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, \
            tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec) ; //获取客户端发送信息的时间，按格式设定成字符串参数
    printf("time:%s",timebuf);
    int flag = 0;
    while(p)//msg信息格式：sender,reciver,mssage
    {
        if (*p != ',')
        {
            reciver[i] = *p;
        }
        else
        {
            ++p;
            break;
        }
        ++i;
        ++p;
    }
    printf ("%s\n",p);
    strcpy(message,p);
    printf("sockfd :%d sendmessage :%s\n", sockfd, buf);//test
    printf("ip :%s port : %d\n", addr, port);//test
    printf("sender:%s, reciver:%s, message:%s\n",sender,reciver,message);
    i = 0;
    if(strcmp(reciver,sender) == 0)
    {
        printf("The reciver cannot be himself\n");
        strcpy(buf,"aThe reciver cannot be himself.\0");
        flag = 2;
    }
    while( strlen(userid[i] ) != 0 && flag == 0)
    {
        if(strcmp(reciver,userid[i]) == 0)
        {  //插入数据到message数据库中
            //int pid;
            if (insertMessage(sender,reciver,message,timebuf))
            {
                printf("insert success!\n");
                strcpy(buf,"asend message success!\0");
            }
            else
            {
                printf("insert error! sql error\n");
                strcpy(buf,"asql error.\0");
            }
            flag = 1; //判断输入的姓名是否正确
        }
        ++i;
    }
    if(flag == 0)
    {
        printf("insert error! reciver`s id error\n");
        strcpy(buf,"areciver`s id error.\n\0");
    }
    Write(sockfd, buf, strlen(buf) + 1);
    bzero(buf, strlen(buf));//test

}
void server_reciverack( char *buf,char *sender)
{
    char msg[3][100];
    int x = 4;
    int row = 0, cow = 0;
    memset(msg,'\0',300);
    while(buf[x]) //数据结构：msg:sender|time|msg
    {
        if(buf[x] != '|')
        {
            msg[row][cow] = buf[x];
            ++cow;
        }

        else
        {
            msg[row][cow] = '\0';
            ++row;
            cow = 0;
        }
        ++x;
    }
    printf("%s,%s,%s,%s\n",msg[0], sender, msg[2], msg[1]);
    updataMessageToHavesend(msg[0], sender, msg[2], msg[1]);
    bzero(buf, strlen(buf));//test

}
/*****************************************************************************************************/
/*!

 */
void server_returnUserId( char *buf, char *loginid, int sockfd)
{
    int i = 0;
    bzero(buf, strlen(buf));//test
    //获取所有id去除原id
    while( strlen(userid[i] ) != 0 )
    {
        if(strcmp(loginid,userid[i]) == 0)
        {
            ++i;
            continue;
        }
        else
        {
            if(i == 0)
                strcat(buf,userid[i]);
            else
            {
                strcat(buf,"~");
                strcat(buf,userid[i]);
            }
        }
        ++i;
    }
    strcat(buf,"\0");
    Write(sockfd, buf, strlen(buf) + 1);
    //printf("return buf:%s\n",buf);
    bzero(buf, strlen(buf));//test
}
