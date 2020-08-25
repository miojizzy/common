#ifndef __H_M_SELECT_SERVER__
#define __H_M_SELECT_SERVER__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>

#include "network/m_event.h"

namespace common{


const int PORT = 8080;
const int BUFF_SIZE = 10;
const int FD_LIST_SIZE = 10;
extern int errno;

const int MAX_MSG_SIZE = 40960;

class ConnectInfo{
public:
    int fd;
    int send_offset;
    std::string recv;
    std::string send;

    ConnectInfo():fd(-1),send_offset(0),recv("[recv]"),send("[send]"){};
    ~ConnectInfo(){close(fd);};
};

class M_SelectServer{
    public:
        M_SelectServer(){};
        ~M_SelectServer(){};

        int init();
        int run();

        int handle_conn(int i);
        int handle_recv(int i);
        int handle_send(int i);


        int listenfd; //监听端口的socket的事件
        struct sockaddr_in ser_addr; // <netinet/in.h>
        std::vector<ConnectInfo*> connfds; // 连接信息
    
        int maxfd; //所有fd中的最大值
        fd_set fds;    //读fd集合

        struct timeval timeout; //本轮selece最大等待的时间
        char buff[BUFF_SIZE+1] ;  //缓冲区

};


};

#endif
