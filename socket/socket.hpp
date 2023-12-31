/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221123
    last:230721
*/

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <mutex>
#include <thread>
#include <iostream>
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

#define socket_print_msg_info
#define socket_print_data_info
#define socket_print_error_info

#define maxSendLength 1024
#define maxRecvLength 1024
#define maxListenLength 1024

namespace buendia
{

class socket_
{
public:
    socket_();
    ~socket_();
    void socket_server(const char *ip,int port);
    void socket_server(const char *ip,int port,char *send_add,int send_length);
    void socket_server(const char *ip,int port,char *send_add,int send_length,bool &flag);
    void socket_client(const char *ip,int port);
    int socket_server_initial(const char *ip,int port);
    int socket_client_initial(const char *ip,int port);
    int socketClientOperate(int socket_fd,char *send_add,int send_length);
    int socketServerOperate(int socket_fd,char *send_add,int send_length);
    void socketGetWord(char *address,int send_length);

public:
    const int BACKLOG = 3;
    uint8_t listen_recvBuf[maxListenLength];
    char recvBuf[maxRecvLength];
    char sendBuf[maxSendLength];
    unsigned char sendBufu[maxSendLength];


};

};

#endif // SOCKET_HPP
