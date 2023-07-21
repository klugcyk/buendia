/*
    文件等级：密一
    author:klug
    献给安德烈尼古拉耶维奇科尔莫哥洛夫
    start:221123
    last:230721
*/

#include "socket.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <fcntl.h>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace buendia
{

socket_::socket_()
{
#ifdef socket_print_msg_info
    printf("socket open...\n");
#endif
}

socket_::~socket_()
{
#ifdef socket_print_msg_info
    printf("socket close...\n");
#endif
}

void socket_::socket_server(const char* server_ip,int server_port)
{
#ifdef socket_print_msg_info
    printf("start the socker server...\n");
#endif
    int listen_sock;
    listen_sock=socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock==-1)
    {
#ifdef socket_printf_error_info
        perror("get listen socket error");
#endif
        exit(1);
    }

    int ret;
    if (setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&ret,sizeof(ret))==-1)
    {
#ifdef socket_printf_error_info
        perror("set_reuse_addr error");
#endif
        exit(1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if(bind(listen_sock, (sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
#ifdef socket_print_error_info
        perror("listen_socket_bind error");
#endif
        exit(1);
    }

    if(listen(listen_sock,BACKLOG)==-1)
    {
#ifdef socket_print_error_info
        perror("listen_socket_listen error");
#endif
        exit(1);
    }

    sockaddr_in client;
    socklen_t addr_size=sizeof(sockaddr_in);
    while(1)
    {
#ifdef socket_print_msg_info
            printf("start accept\n");
#endif
        int socket_accept=accept(listen_sock, (sockaddr *)&client, &addr_size);
        if(socket_accept==-1)
        {
#ifdef socket_print_error_info
            printf("accept fail!!!\n");
#endif
            continue;
        }
        else
        {
#ifdef socket_print_msg_info
            printf("accept success...\n");
#endif
        }
        int buf_size=1024;

        while((recv(socket_accept,recvBuf,buf_size,0))>0)
        {
            send(socket_accept,sendBuf,15,0);
            //std::cout<<"recv:="<<recvBuf<<std::endl;
        }
        close(socket_accept);
    }
}

void socket_::socket_client(const char * ip,int port)
{
    int socket_fd=socket_client_initial(ip,port);

    char sendBuf[]={};

    while(1)
    {
        std::string word;
        send(socket_fd,sendBuf,strlen(sendBuf),0);
        char recvBuf[1024]={};
        recv(socket_fd,recvBuf,1024,0);
        std::cout << "back:=" << recvBuf  << std::endl;
    }
}

int socket_::socket_server_initial(const char * ip,int port)
{
#ifdef socket_print_msg_info
    printf("start the socker server...\n");
#endif
    int listen_sock;
    listen_sock=socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock==-1)
    {
#ifdef socket_printf_error_info
        perror("get listen socket error");
#endif
        exit(1);
    }

    int ret;
    if (setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&ret,sizeof(ret))==-1)
    {
#ifdef socket_printf_error_info
        perror("set_reuse_addr error");
#endif
        exit(1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if(bind(listen_sock, (sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
#ifdef socket_print_error_info
        perror("listen_socket_bind error");
#endif
        exit(1);
    }

    if(listen(listen_sock,BACKLOG)==-1)
    {
#ifdef socket_print_error_info
        perror("listen_socket_listen error");
#endif
        exit(1);
    }

    sockaddr_in client;
    socklen_t addr_size=sizeof(sockaddr_in);
    //while(1)
    {
#ifdef socket_print_msg_info
        printf("start accept\n");
#endif
        int socket_accept=accept(listen_sock, (sockaddr *)&client, &addr_size);
        if(socket_accept==-1)
        {
#ifdef socket_print_error_info
            printf("accept fail!!!\n");
#endif
            //continue;
        }
        else
        {
#ifdef socket_print_msg_info
            printf("accept success...\n");
            return socket_accept;
#endif
        }
    }
    return 0;
}

int socket_::socket_client_initial(const char* ip_address,int port)
{
#ifdef socket_print_msg_info
    printf("start socket client...\n");
#endif
    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        std::cout<<"socket create fail!!!"<<std::endl;
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); //the server port
    addr.sin_addr.s_addr = inet_addr(ip_address); //the server ip

    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    if(res==-1)
    {
        printf("bind link fail!!!\n");
        exit(-1);
    }
    printf("bind link success...\n");
    return socket_fd;
}

void socket_::socket_server(const char *ip,int port,char *send_add,int send_length,bool &flag)
{
    int listen_sock=socket_server_initial(ip,port);
    sockaddr_in client;
    socklen_t addr_size=sizeof(sockaddr_in);
    while(1)
    {
#ifdef socket_print_msg_info
        printf("accepting...\n");
#endif
        int socket_accept=-1;
        if(socket_accept==-1)
        {
            socket_accept=accept(listen_sock, (sockaddr *)&client, &addr_size);
        }

        if(socket_accept==-1)
        {
            continue;
#ifdef socket_print_error_info
            printf("accept fail!!!\n");
#endif
        }
        else
        {
#ifdef socket_print_msg_info
            printf("accept success...\n");
#endif
        }

        //读取char数组
        char send_buf[send_length];
        for(int i=0;i<send_length;i++)
        {
            send_buf[i]=*send_add;
            send_add++;
        }
        char recv_buf[100];

        int buf_size=send_length;
        while((recv(socket_accept,recv_buf,buf_size,0))>0)
        {
            int send_cond=0;
            if(flag)
            {
                send_cond=send(socket_accept,send_buf,strlen(send_buf),0);
                flag=0;
            }

            if(send_cond==-1)
            {
                socket_accept=-1;
                break;
            }
            usleep(100000);
        }
        close(socket_accept);
    }
}

void socket_::socket_server(const char *ip,int port,char *send_add,int send_length)
{
    int listen_sock=socket_server_initial(ip,port);
    sockaddr_in client;
    socklen_t addr_size=sizeof(sockaddr_in);
    while(1)
    {
#ifdef socket_print_msg_info
        printf("accepting...\n");
#endif
        int socket_accept=-1;
        if(socket_accept==-1)
        {
            socket_accept=accept(listen_sock, (sockaddr *)&client, &addr_size);
        }

        if(socket_accept==-1)
        {
            continue;
#ifdef socket_print_error_info
            printf("accept fail!!!\n");
#endif
        }
        else
        {
#ifdef socket_print_msg_info
            printf("accept success...\n");
#endif
        }

        //读取char数组
        char send_buf[send_length];
        for(int i=0;i<send_length;i++)
        {
            send_buf[i]=*send_add;
            send_add++;
        }
        char recv_buf[100];

        int buf_size=send_length;
        while((recv(socket_accept,recv_buf,buf_size,0))>0)
        {
            int send_cond=send(socket_accept,send_buf,strlen(send_buf),0);
            if(send_cond==-1)
            {
                socket_accept=-1;
                break;
            }
            usleep(100000);
        }
        close(socket_accept);
    }
}

/*
    socket客户端发送数据
    @socket_fd:socket创建编号
    @send_add:发送数据的首地址
    @send_length:发送数据长度
    @ret:返回值，接受到服务器反馈数据大小
*/
int socket_::socketClientOperate(int socket_fd,char *send_add,int send_length)
{
    // 获取数据到sendbuf
    socketGetWord(send_add,send_length);
    int ret=send(socket_fd,sendBuf,strlen(sendBuf),0);
    // 读取下位机的返回值
    if(ret<0)
    {
#ifdef socket_print_error_info
    std::cout<<"send error..."<<std::endl;
#endif
    }
    else
    {
        ret=recv(socket_fd,recvBuf,1024,0);
    }

#ifdef socket_print_data_info
    std::cout << "back:=" << recvBuf  << std::endl;
#endif

    return ret;
}

/*
    socket服务器发送数据
    @socket_fd:socket创建编号
    @send_add:发送数据的首地址
    @send_length:发送数据长度
    @ret:返回值，接受到客户端反馈数据大小
*/
int socket_::socketServerOperate(int socket_fd,char *send_add,int send_length)
{
    return 0;
}

/*
    保存数据到sendBuf中
*/
void socket_::socketGetWord(char *address,int send_length)
{

}

};
