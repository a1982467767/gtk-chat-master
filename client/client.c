/*
Copyright (C) 2018 SunChen
*/
#include "head.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>


static char userid[10][10] = {"\0"} ;
static char *myid;
static char *mypasswd;
static int sockfd;

GtkWidget *connectwindow;
GtkWidget *loginwindow;
GtkWidget *sendwindow;

GtkWidget *status;//ADD

struct reg_text {
    GtkEntry * id;
    GtkEntry * passwd;
};

void closeApp(GtkWidget *window, gpointer data)
{
    //printf("Destroy\n");
    gtk_main_quit();
}

void closeSendWin(GtkWidget *window, gpointer data)
{
    //printf("Destroy\n");
    Write( sockfd, "", 0);
    gtk_main_quit();
}

void connect_button_clicked(GtkWidget *button, gpointer data)
{
    //int sockfd;
    struct sockaddr_in servaddr;
    int n;

    const char * server_text  = strlen(gtk_entry_get_text(GTK_ENTRY((GtkWidget *) data))) != 0 ?
                gtk_entry_get_text(GTK_ENTRY((GtkWidget *) data)) : "127.0.0.1";
    //jadge

    bzero(&servaddr,sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    if((n=sock_pton(server_text,(SA *)&servaddr))==-1)
    {
        printf("wrong formatted ip address.\n");
        return;
    }
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.101");
    servaddr.sin_addr.s_addr = inet_addr(server_text);
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
        /*这里是模仿作者对socket()进行简单的包装*/
    {
        printf("socket() error:%s\n",strerror(errno));
        return;
    }

    if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))<0)
    {
        printf("connect() error:%s\n",strerror(errno));
        close(sockfd);
        return;
    }
    printf("server ip:%s connnect success.\n" ,server_text);
    gtk_widget_destroy (connectwindow);
    return;
}


void login_button_clicked(GtkWidget *button, gpointer data)
{
    struct reg_text * pRt = (struct reg_text *)data;
    char buf[MAXSIZE];
    int n;
    const char * userid_text  = strlen(gtk_entry_get_text(GTK_ENTRY(pRt->id))) != 0?
                gtk_entry_get_text(GTK_ENTRY(pRt->id)) : "159074277";
    const char * password_text = strlen(gtk_entry_get_text (GTK_ENTRY(pRt->passwd))) != 0?
                gtk_entry_get_text(GTK_ENTRY(pRt->passwd)) : "123456\0";
    strcpy(buf,userid_text);
    strcat(buf,"/");
    strcat(buf,password_text);
    Write( sockfd, buf, strlen(buf) + 1); //将登录信息发给服务器进行登录
    printf("sender login message to server:%s\n",buf);
    bzero(buf, strlen(buf));//test
    for(;;)
    {
        while((n = read(sockfd, buf, MAXSIZE)) != -1)
        {
            //printf("%s\n",buf);
            if(n == 0)
            {
                perror("connect reset!\n");
                close(sockfd);
                exit(0);
            }
            else if(strncmp(buf,"sameid",6) == 0)
            {
                perror("login failed! same id\n");
                return;
            }
            else if(strncmp(buf,"failed",6) == 0)
            {
                perror("login failed!\n");
                return;
            }
            else if(strncmp(buf,"success",7) == 0)
            {
                printf ("%s login success\n", userid_text);
                myid = (char * )malloc(10);
                mypasswd = (char *)malloc(18);
                strcpy(myid,userid_text);
                strcpy(mypasswd,password_text);
                bzero(buf, strlen(buf));
                sprintf(buf,"getatherid~%s",myid);
                buf[strlen(buf)] = '\0';
                Write( sockfd, buf, strlen(buf) + 1 );
                //gtk_widget_destroy (loginwindow);
            }
            else if(strlen(buf) == 9 || buf[9] =='~' )
            {

                int x = 0;
                int row = 0, cow = 0;
                while(buf[x])
                {
                    if(buf[x] != '~')
                    {
                        userid[row][cow] = buf[x];
                        ++cow;
                    }
                    else
                    {
                        userid[row][cow] = '\0';
                        ++row;
                        cow = 0;
                    }
                    ++x;
                }
                gtk_widget_destroy (loginwindow);
                return;

            }
            else if (strncmp(buf,"msg:",4) == 0)
            {
                printf("get server message:%s\n",buf);
            }
            else
                Write(fileno(stderr), buf, n);
            bzero(buf, n);//test
        }
    }
}



int connectWindows(int argc, char *argv[])
{
    GtkWidget *server_label;
    GtkWidget *server_entry;
    GtkWidget *ok_button;
    GtkWidget *hbox1;
    GtkWidget *vbox;

    gtk_init(&argc, &argv);
    connectwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(connectwindow), "Client");
    gtk_window_set_position(GTK_WINDOW(connectwindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(connectwindow), 200, 200);

    g_signal_connect(G_OBJECT(connectwindow), "destroy", G_CALLBACK(closeApp), NULL);

    server_label = gtk_label_new("Server IP: ");
    server_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(server_entry),"127.0.0.1");
    ok_button = gtk_button_new_with_label("Connect");

    g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(connect_button_clicked),
                     server_entry);

    hbox1 = gtk_hbox_new(TRUE, 5);
    vbox = gtk_vbox_new(FALSE, 10);

    gtk_box_pack_start(GTK_BOX(hbox1), server_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), server_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(connectwindow), vbox);

    gtk_widget_show_all(connectwindow);
    gtk_main();

    return 0;
}

int loginwindows(int argc, char *argv[])
{

    GtkWidget *userid_label, *password_label;
    GtkWidget *userid_entry, *password_entry;
    GtkWidget *ok_button;
    GtkWidget *hbox1, *hbox2;
    GtkWidget *vbox;
    struct reg_text rt;
    //getUserId();
    gtk_init(&argc, &argv);
    loginwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(loginwindow), "Login");
    gtk_window_set_position(GTK_WINDOW(loginwindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(loginwindow), 200, 200);

    g_signal_connect(G_OBJECT(loginwindow), "destroy", G_CALLBACK(closeApp), NULL);

    userid_label = gtk_label_new("Login: ");
    password_label = gtk_label_new("Password: ");
    userid_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(userid_entry),"159074277");
    gtk_entry_set_text(GTK_ENTRY(password_entry),"123456");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    ok_button = gtk_button_new_with_label("OK");

    g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(login_button_clicked),
                     (gpointer)&rt);

    hbox1 = gtk_hbox_new(TRUE, 5);
    hbox2 = gtk_hbox_new(TRUE, 5);
    vbox = gtk_vbox_new(FALSE, 10);

    rt.id = GTK_ENTRY(userid_entry);
    rt.passwd = GTK_ENTRY(password_entry);

    gtk_box_pack_start(GTK_BOX(hbox1), userid_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), userid_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), password_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), password_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(loginwindow), vbox);

    gtk_widget_show_all(loginwindow);
    gtk_main();

    return 0;
}



GtkWidget *sendto_label, *message_label;
GtkWidget *sendto_entry, *message_entry;
GtkWidget *ok_button;
GtkWidget *hbox1, *hbox2;
GtkWidget *vbox;

#if 1
GtkWidget *frame;
GtkWidget *sends;
GtkWidget *vbox_gui;
GtkWidget *hbox_gui;
GtkWidget *progress;
GtkWidget *fileframe;
GtkWidget *filelabel;
GtkWidget *browse;
GtkWidget *filebox;
#endif

void send_button_clicked(GtkWidget *button, gpointer data)
{
    char buf[MAXSIZE];
    int pipes[2];
    int  n;

    struct reg_text * pRt = (struct reg_text *)data;
    const char * sendto_text  = (char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(sendto_entry));
    const char * message_text = gtk_entry_get_text(GTK_ENTRY(message_entry));
    if(strlen(message_text) == 0)
    {
        gtk_label_set_text(GTK_LABEL(status),"message can not null.");
        return;
    }
    if (pipe(pipes) == 0)
    {
        write(pipes[1],sendto_text,strlen(sendto_text));
        write(pipes[1],",",1); //写入一个空格，用来分割
        write(pipes[1],message_text,strlen(message_text));
        write(pipes[1],"\0",1); //写入一个空格，用来分割
        close(0);
        dup(pipes[0]);
    }
    n = read(pipes[0], buf, MAXSIZE);
    Write( sockfd, buf, strlen(buf)+1 );
    return;
}


char *getfilename()
{

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Select Location",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        printf("%s",(char *)filename);
        gtk_widget_destroy (dialog);
        return filename;
    }
}

char Read_SC(int msecs)
{
    struct timeval val;
    static int i=0;
    fd_set rset;
    char c;
    int fd=sockfd;

    val.tv_sec = (msecs / 1000);
    val.tv_usec = (msecs % 1000) * 1000;
    FD_ZERO(&rset);
    FD_SET(sockfd,&rset);
    switch (select(fd + 1, &rset,0,0,&val))
    {
    case 0: // timeout - treat it like an error.
        return 0;
    case 1: // success - input activity detected.
        printf("111\n");
        read(fd, &c, 1);
        if (c == ' ')
            return 0;
        printf("%c\n",c);
        return c;
    default: // error
        return 0;
    }
    printf("9\n");

}

#define CHATNUM 5
GtkWidget *chatwindow[CHATNUM];
GtkWidget *chatvbox[CHATNUM];
GtkWidget *msg_lable[CHATNUM];
void initchat()
{
    int i ;
    for (i = 0; i < CHATNUM; i++)
    {
        chatwindow[i] = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(chatwindow[i]), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(chatwindow[i]), 250, 300);
        msg_lable[i] = gtk_label_new("NULL");
        chatvbox[i] = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(chatvbox[i]), msg_lable[i], TRUE, TRUE, 5);
        gtk_container_add(GTK_CONTAINER(chatwindow[i]), chatvbox[i]);
    }
}
int chatWindows()
{
#if 1
    int n = 0, i;
    char buf[MAXSIZE];
    char resendMsg[2000] = {"\0"};
    int isexist = 0;
    static char exist[5][10] = {"\0"};  //保存历史聊天者
    static int existnum = 0; // 保存历史聊天者个数
    while((n = read(sockfd, buf, MAXSIZE)) != -1)
    {
        printf("reciver:%s\n",buf);
        if(n == 0)
        {
            perror("connect reset!\n");
            close(sockfd);
            return 0;
        }
        else if(strncmp(buf,"msg:",4) == 0)
        {
            //int isexist = 0;
            char msg[3][100];
            int x = 4;
            int row = 0, cow = 0;
            memset(msg,'\0',300);
            printf("senderMEsssage:%s\n",buf);
            Write( sockfd, buf, strlen(buf));
            while(buf[x])
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
            for (i = 0 ;i < existnum; ++i)
            {
                if (strcmp(msg[0],exist[i]) == 0 )
                {
                    strcat(resendMsg,gtk_label_get_text(GTK_LABEL(msg_lable[i]))); //后缀次数过多导致堆栈溢出，这里要进行削减
                    isexist = 1;
                    break;
                }
            }
            memset(buf,'\0',128);
            printf("messge:%s/%s/%s\n",msg[0],msg[1],msg[2]);
            strcat(resendMsg,msg[0]);
            strcat(resendMsg," :(");
            strcat(resendMsg,msg[1]);
            strcat(resendMsg,")  :[ ");
            strcat(resendMsg,msg[2]);
            strcat(resendMsg," ]\n");
            printf("%s",resendMsg);
            printf("shuchu:%s\n",msg[0]);

            if (isexist == 0)
            { //不存在与该成员的聊天窗口，创建聊天窗口
                char title[30];
                strcpy(title,"s:");
                strcat(title,msg[0]);
                strcat(title,"->");
                strcat(title,myid);
                strcpy(exist[i], msg[0]);
                gtk_window_set_title(GTK_WINDOW(chatwindow[i]), title);
                g_signal_connect(G_OBJECT(chatwindow[i]), "destroy", G_CALLBACK(closeApp), NULL);
                gtk_label_set_text(GTK_LABEL(msg_lable[i]),resendMsg);
                gtk_widget_show_all(chatwindow[i]);
                ++existnum;
            }
            else
            {
                gtk_label_set_text(GTK_LABEL(msg_lable[i]),resendMsg);
            }

            bzero(buf, strlen(buf));//test
        }
        return 1;
    }
#endif
}

void choosefile(GtkWidget *browse)
{
    GtkWidget *dialog;
    dialog=gtk_file_chooser_dialog_new("Open File",GTK_WINDOW(sendwindow),GTK_FILE_CHOOSER_ACTION_OPEN,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                       NULL);
    if(gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gtk_label_set_text(GTK_LABEL(filelabel),filename);
        //strcpy(filename,file);
        gtk_widget_destroy (dialog);
        g_free(filename);
    }
}

int sendfile(GtkWidget *send)
{
#if 0
    char *string=(char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(clients));

    if(string==NULL)
        return 0;
    int sockfd=0;
    int i=0;
    while(string[i]!=' ')
    {
        sockfd=sockfd*10+(int)(string[i]-48);
        i++;
    }

#endif
    if (gtk_label_get_text(GTK_LABEL(filelabel))!=NULL)
    {
        char *filename=(char *)gtk_label_get_text(GTK_LABEL(filelabel));
        FILE *ptr;
        char c;
        int size=0;
        int loc=0;
        int d;


        ptr=fopen(filename,"r");
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress),1);
        c='0';
        write(sockfd,&c,1);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress),"Sending File");
        while(fscanf(ptr,"%c",&c)!=EOF)
        {
            d=(int)c;
            write(sockfd,&d,sizeof(int));
        }
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress),"File transfer completed");
        d=-999;
        write(sockfd,&d,sizeof(int));
        fclose(ptr);
        //gtk_widget_destroy(status);
        //g_free (filename);
        return 1;
    }

    //g_free(string);

    return 1;
}
int check(gpointer data)
{
    char c;
    int d;
    int n = 0;
    char buf[MAXSIZE];
    FILE *ptr;
    char *fname;
    //printf("test ");
    if (1)
    {
        c=Read_SC(100);
        //printf("%c\n",c);
        if(c==0)
            return 1;
    }
    else
        return 0;
    switch(c)
    {
    case '0':
        fname=getfilename();
        printf("%s\n",fname);
        printf("Opening file\n");
        //gtk_label_set_text(GTK_LABEL(fstatus),NULL);
        ptr=fopen(fname,"w");
        while(1)
        {

            read(sockfd,&d,sizeof(int));
            //printf("%s",buf);
            if(d==(-999))
            {
                fclose(ptr);
                printf("\nFile closed\n");
                //flag=0;
                free(fname);
                //gtk_label_set_text(GTK_LABEL(fstatus),"File Saved");
                return 1;
            }
            //read(sock,&c,1);
            printf("%c",d);
            fprintf(ptr,"%c",d);
        }

        //return 1;
    case '1':
        close(sockfd);
        //gtk_widget_destroy_all(window);
        gtk_main_quit();
        exit(0);
    case 'a' :
    default:
        chatWindows(buf);
    }
    return 1;
    printf("over\n");

}
void initialize()
{

    sendwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sendwindow), myid);
    gtk_window_set_position(GTK_WINDOW(sendwindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(sendwindow), 300, 400);
    sendto_label = gtk_label_new("SendTo: ");
    message_label = gtk_label_new("Message: ");
    sendto_entry = gtk_combo_box_text_new();
    message_entry = gtk_entry_new();
    ok_button = gtk_button_new_with_label("Send");
#if 1
    frame=gtk_frame_new(NULL);
    sends=gtk_button_new_with_label("Send");
    vbox_gui=gtk_vbox_new(1,5);
    hbox_gui=gtk_hbox_new(1,5);
    progress=gtk_progress_bar_new();
    fileframe=gtk_frame_new("Select file");
    filelabel=gtk_label_new(NULL);
    browse=gtk_button_new_with_label("Browse");
    filebox=gtk_hbox_new(0,0);
#endif
    initchat();

}
void signals()
{
    g_signal_connect(G_OBJECT(sendwindow),"destroy",gtk_main_quit,NULL);
    g_timeout_add_seconds(1,check,NULL);
    g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(send_button_clicked),
                     NULL);
    g_signal_connect(browse,"clicked",G_CALLBACK(choosefile),NULL);
    g_signal_connect(sends,"clicked",G_CALLBACK(sendfile),NULL);
    //g_timeout_add(500,check,NULL);


}

void packing()
{
    int i;
    hbox1 = gtk_hbox_new(TRUE, 5);
    hbox2 = gtk_hbox_new(TRUE, 5);
    vbox = gtk_vbox_new(FALSE, 10);
    i = 0 ;
    while( strlen(userid[i] ) != 0 ) //将用户信息加载到选择框
    {
        if(strcmp(myid,userid[i]) == 0)
        {
            ++i;
            continue;
        }
        else
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(sendto_entry),userid[i]);
        ++i;
    }
    gtk_box_pack_start(GTK_BOX(hbox1), sendto_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), sendto_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), message_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), message_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(sendwindow), vbox);
    //gtk_box_pack_start(GTK_BOX(vbox),progress,0,0,5);
    //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress),0);
#if 1
    gtk_box_pack_start(GTK_BOX(vbox_gui),fileframe,0,0,5);
    gtk_container_add(GTK_CONTAINER(fileframe),filebox);
    gtk_box_pack_start(GTK_BOX(filebox),filelabel,0,0,5);
    gtk_box_pack_start(GTK_BOX(filebox),browse,0,0,5);
    gtk_box_pack_start(GTK_BOX(vbox_gui),hbox_gui,1,1,0);
    gtk_box_pack_start(GTK_BOX(hbox_gui),sends,0,1,5);
    gtk_box_pack_start(GTK_BOX(vbox_gui),progress,0,0,5);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress),0);
    gtk_container_add(GTK_CONTAINER(sendwindow),frame);
    gtk_container_add(GTK_CONTAINER(frame),vbox_gui);
    //gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress),"Disabled");
#endif
    gtk_widget_show_all(sendwindow);


}
/*****************************************************************************************************/
/*!

 */
int main(int argc,char * argv[])
{
    connectWindows(argc,argv);
    loginwindows(argc,argv);

    //sendMassageWindows(argc,argv);
    initialize();
    signals();
    packing();
    gtk_main();


}
#if 0 //多线程
/*****************************************************************************************************/
/*!

 */
int sendMassageWindows(int argc, char *argv[])
{
    GtkWidget *sendto_label, *message_label;
    GtkWidget *sendto_entry, *message_entry;
    GtkWidget *ok_button;
    GtkWidget *hbox1, *hbox2;
    GtkWidget *vbox;

    struct reg_text rt;
    int i;
    pthread_t t1;

    gtk_init(&argc, &argv);
    sendwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sendwindow), "Communication");
    gtk_window_set_position(GTK_WINDOW(sendwindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(sendwindow), 200, 300);

    g_signal_connect(G_OBJECT(sendwindow), "destroy", G_CALLBACK(closeSendWin), NULL);

    sendto_label = gtk_label_new("SendTo: ");
    message_label = gtk_label_new("Message: ");
    sendto_entry = gtk_combo_box_text_new();
    message_entry = gtk_entry_new();

    ok_button = gtk_button_new_with_label("Send");

    g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(send_button_clicked),
                     (gpointer)&rt);

    hbox1 = gtk_hbox_new(TRUE, 5);
    hbox2 = gtk_hbox_new(TRUE, 5);
    vbox = gtk_vbox_new(FALSE, 10);

    rt.id = GTK_COMBO_BOX_TEXT(sendto_entry);
    rt.passwd = GTK_ENTRY(message_entry);


    gtk_box_pack_start(GTK_BOX(hbox1), sendto_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), sendto_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), message_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), message_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(sendwindow), vbox);

    i = 0 ;
    while( strlen(userid[i] ) != 0 ) //将用户信息加载到选择框
    {
        if(strcmp(myid,userid[i]) == 0)
        {
            ++i;
            continue;
        }
        else
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(sendto_entry),userid[i]);
        ++i;
    }
    //g_timeout_add(100,check,NULL);
    gtk_widget_show_all(sendwindow);
#if 0
    int err = pthread_create(&t1,NULL,str_cli,(void*)NULL);
    if(err!=0)
    {
        printf("thread_create Failed:%s\n",strerror(errno));
    }
    else
    {
        printf("thread_create success\n");
    }
#endif
    gtk_main();

    return 0;

}
/*****************************************************************************************************/
/*!

 */
void str_cli(void *argv)
{
    const int on=1;
    char buf[MAXSIZE];
    int  n;
    int nfds, epfd;
    struct epoll_event ev, events[Client_MAX_EVENTS];
    int i;\n\n\n
            FILE * fp = stdin;
    GtkWidget *chatwindow;
    GtkWidget *vbox;
    GtkWidget *msg_lable;
    char resendMsg[300] = {"\0"};
    chatwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(chatwindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(chatwindow), 250, 300);
    msg_lable = gtk_label_new("NULL");
    vbox = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), msg_lable, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(chatwindow), vbox);
    //char* exist[5] = {'\0'};  //保存历史聊天者
    //int existnum = 0; // 保存历史聊天者个数
    Setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&on,sizeof(on));
    epfd = Epoll_create( Client_MAX_EVENTS );
    bzero( &ev, sizeof(struct epoll_event) );
    ev.data.fd = fp;
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

            if( events[i].data.fd == fp )
            {

                n = read(fp, buf, MAXSIZE);

                //printf ("shuru:%s\n",buf);
                if(n == 0 || strcmp(buf ,"exit\n") == 0)
                {
                    close(sockfd);
                    return;
                }
                else
                {
                    Write( sockfd, buf, strlen(buf)+1 );
                }
                bzero(buf, strlen(buf));//test
                fflush(0);//test
            }
            if(events[i].data.fd==sockfd)
            {
                /*if n==-1 ,it said that read() return errno==EAGAIN*/
                while((n = Read(sockfd, buf, MAXSIZE)) != -1)
                {
                    printf("reciver:%s\n",buf);
                    if(n == 0)
                    {
                        perror("connect reset!\n");
                        close(sockfd);
                        return;
                    }
                    else if(strncmp(buf,"msg:",4) == 0)
                    {
                        //int isexist = 0;
                        char msg[3][100];
                        int x = 4;
                        int row = 0, cow = 0;
                        memset(msg,'\0',300);
                        while(buf[x])
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
                        memset(buf,'\0',128);
                        printf("messge:%s/%s/%s\n",msg[0],msg[1],msg[2]);
                        strcat(resendMsg,msg[0]);
                        strcat(resendMsg," :(");
                        strcat(resendMsg,msg[1]);
                        strcat(resendMsg,")  :[ ");
                        strcat(resendMsg,msg[2]);
                        strcat(resendMsg," ]\n");
                        printf("%s",resendMsg);
                        //                        for (i = 0 ;i < existnum; ++i)
                        //                            if (strcmp(msg[0],exist[i] == 0 ))
                        //                            {
                        //                                isexist = 1;
                        //                                gtk_label_set_text(GTK_LABEL(msg_lable),resendMsg);
                        //                                break;
                        //                            }
                        //                        if (isexist == 0)
                        { //不存在与该成员的聊天窗口，创建聊天窗口
                            gtk_window_set_title(GTK_WINDOW(chatwindow), myid);
                            g_signal_connect(G_OBJECT(chatwindow), "destroy", G_CALLBACK(closeApp), NULL);
                            gtk_label_set_text(GTK_LABEL(msg_lable),resendMsg);
                            gtk_widget_show_all(chatwindow);

                            //                            strcpy(exist[existnum],msg[0]);
                            //                            ++existnum; //不能大于5

                            //gtk_main();
                            /*
                            gtk_window_set_title(GTK_WINDOW(chatwindow[existnum]), msg[0]);
                            vbox[existnum] = gtk_vbox_new(FALSE, 10);
                            gtk_box_pack_start(GTK_BOX(vbox[existnum]), msg_lable[existnum], TRUE, TRUE, 5);
                            gtk_label_set_text(GTK_LABEL(msg_lable[existnum]),resendMsg);
                            gtk_container_add(GTK_CONTAINER(chatwindow[existnum]), vbox[existnum]);
                            gtk_widget_show_all(chatwindow[existnum]);
                            */
                        }

                        bzero(buf, strlen(buf));//test
                    }
                }
            }
        }

    }

}
#endif
