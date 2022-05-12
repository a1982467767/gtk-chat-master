/*
Copyright (C) 2018 SunChen
*/
#include "mysqldb.h"
#include "common.h"

#include </usr/include/mysql/mysql.h>  //我的机器上该文件在/usr/include/mysql下


/*****************************************************************************************************/
/*!
 * \brief getUserId
 * \return Get all user id list from mysql chat.chatuser
 */
int getUserId(char userid[10][10])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    int row_i = 0;
    //char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    if(mysql_query(sock,SELECT_ID))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        //printf("User id is: %s\n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        strcpy(userid[row_i],(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]));
        ++row_i;
    }
    mysql_free_result(res);
    mysql_close(sock);
    //exit(0);
    return 0;   //. 为了兼容大部分的编译器加入此行
}

/*****************************************************************************************************/
/*!
 * \brief getOnlineUserId
 * \return Get all online user id list from mysql chat.chatuser
 */
int getOnlineUserId(char onlineuserid[10][10])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    int row_i = 0;
    //char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    if(mysql_query(sock,ONLINE))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User id is: %s\n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        strcpy(onlineuserid[row_i],(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]));
        ++row_i;
    }
    mysql_free_result(res);
    mysql_close(sock);
    //exit(0);
    return 0;   //. 为了兼容大部分的编译器加入此行
}
/*****************************************************************************************************/
/*!
 * \brief getUserName
 * \return Get all user name list from mysql chat.chatuser
 */
int getUserName(char username[10][10])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    int row_i = 0;
    //char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    if(mysql_query(sock,SELECT_NAME))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User id is: %s\n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        strcpy(username[row_i],(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]));
        ++row_i;
    }
    mysql_free_result(res);
    mysql_close(sock);
    //exit(0);
    return 0;   //. 为了兼容大部分的编译器加入此行
}

/*****************************************************************************************************/
/*!
 * \brief insertMessage
 * \param sender
 * \param reciver
 * \param message
 * \return insert Message into chat.message
 */
int insertMessage(char sender[],char reciver[], char message[], char time[])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        return 0;
    }
    sprintf(qbuf, INSERT, sender, reciver, message, time);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return 0;
    }
    mysql_close(sock);
    return 1;
}

/*****************************************************************************************************/
/*!

 */
int updataChatUser(char sender[], char ip[], int port)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        return -1;
    }
    sprintf(qbuf, UPDATECHATUSER, ip, port, sender);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return -1;
    }
    mysql_close(sock);
    return 1;
}

/*****************************************************************************************************/
/*!
 * \brief updataChatUser_offline
 * \param ip
 * \param port
 * \return
 */
int updataChatUser_offline(char ip[], int port)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        return 0;
    }
    sprintf(qbuf, UPDATECHATUSEROFF, ip, port);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return 0;
    }
    mysql_close(sock);
    return 1;
}


/*****************************************************************************************************/
/*

 */

char * getUserNameByIp(char ipaddr[], int port)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    sprintf(qbuf, SELECT_NAME_BY_IP ,ipaddr,port);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User name is: %s \n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        mysql_free_result(res);
        mysql_close(sock);
        return(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]);
    }
    return "NULL";

}

/*****************************************************************************************************/
/*

 */
char * getPasswdById(const char * userid_text)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    sprintf(qbuf,SELECT_PASSWD_BY_ID,userid_text);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User passwd is: %s \n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        mysql_free_result(res);
        mysql_close(sock);
        return(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]);
    }
}

/*****************************************************************************************************/
/*

 */
char * getNameById(const char * userid_text)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    sprintf(qbuf,SELECT_NAME_BY_ID,userid_text);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User name is: %s \n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        mysql_free_result(res);
        mysql_close(sock);
        return(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0]);
    }
}

/*****************************************************************************************************/
/*

 */
int Login(char id[], char passwd[] , char ipaddr[],int port)
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        return -1;
    }

    sprintf(qbuf, LOGIN ,id, passwd);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return -1;
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        return -1;
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User name is: %s \n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        if(updataChatUser(id,ipaddr,port) == 0)
        {
            return 0;
        }
        mysql_free_result(res);
        mysql_close(sock);
        return 1;
    }
    return 0;

}

/*****************************************************************************************************/
/*!

 */
int getNoSendMessage(char userid[], struct msgstrcut result[10])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_FIELD *fd ;     //包含字段信息的结构
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    int row_i = 0;
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    sprintf(qbuf, NOSENDMESSAGE ,userid);
    //printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        printf("User id is: %s\n",(((row[0]==NULL)&&(!strlen(row[0]))) ? "NULL" : row[0])) ;
        strcpy(result[row_i].sender, row[0]);
        strcpy(result[row_i].reciver,row[1]);
        strcpy(result[row_i].msg,row[2]);
        strcpy(result[row_i].time,row[3]);
        ++row_i;
//        if (row_i >= 10) //超过发送缓存长度，每次检索只能发送十条信息
//        {
//            break;
//        }
    }
    mysql_free_result(res);
    mysql_close(sock);
    return 0;   //. 为了兼容大部分的编译器加入此行

}

/*****************************************************************************************************/
/*!

 */
int updataMessageToHavesend(char sender[], char reciver[], char msg[], char time[])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        return 0;
    }
    sprintf(qbuf, UPDATEMSGHAVESEND, sender, reciver, msg, time);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return 0;
    }
    mysql_close(sock);
    return 1;

}

/*****************************************************************************************************/
/*!

 */
int isOnline(char id[])
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    MYSQL_RES *res;       //查询结果集，结构类型
    MYSQL_ROW row ;       //存放一行查询结果的字符串数组
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        exit(1);
    }

    sprintf(qbuf,ISONLINE,id);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res = mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
        exit(1);
    }

    while (row = mysql_fetch_row(res))
    {
        mysql_free_result(res);
        mysql_close(sock);
        return 1 ; //在线
    }
    return 0; //不在线
}

/*****************************************************************************************************/
/*!

 */
int offOnline()
{
    MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
    char qbuf[160];      //存放查询sql语句字符串

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql,"localhost","root","yushan2d","chat",0,NULL,0)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("");
        return 0;
    }
    sprintf(qbuf, OFFLINE);
    printf ("%s\n",qbuf);
    if(mysql_query(sock,qbuf))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return 0;
    }
    mysql_close(sock);
    return 1;
}

