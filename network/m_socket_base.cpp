#include"network/m_socket_base.h"
#include <arpa/inet.h>
#include <sys/socket.h>

namespace common{


bool M_SocketBase::init(const char* ip, int port, bool nonblock){
    // 生成socket 
    if (-1 == (fd = socket(PF_INET, SOCK_STREAM, 0)) ){//netinet/in.h
        printf("create socket error!\n");
        return false;
    }

    // 设置事件为非阻塞

    if(nonblock){
        if(!set_fd_nonblock(fd)){
            printf("set_fd_nonblock error");
        }
    }
    
    // 生成服务端地址
    memset(&addr, 0, sizeof(addr));//string.h
    addr.sin_family = AF_INET;                  //链接协议
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);   //链接地址范围
    if( 0 > inet_aton(ip, &addr.sin_addr)){
        printf("parse ip error");
    }
    addr.sin_port = htons(8800);                //链接端口

    // 将上面的地址绑定到到 事件 
    if (-1 == bind(fd, (struct sockaddr*)&addr, sizeof(addr))){
        printf("bind fd and server_addr error!\n");
        return false;
    }
    
    return true;
};


void M_SocketBase::run(){
    //开始监听 并设置可以保持10个请求处于等待
    if (-1 == listen(fd, 10)){ 
        printf("listen fd error\n");
        return;
    }
    conn_list.push_back(M_ConnectInfo::create());
    M_ConnectInfo& conn = *conn_list.back();
    socklen_t cli_len = sizeof(conn.addr);//地址结构的长度
    printf("waiting for connect ...\n");
    if (-1 == (conn.fd = accept(fd, 
                    (struct sockaddr*)&conn.addr, 
                    &cli_len)) ){
        //阻塞接收连接(非阻塞应该在之前设置到监听事件)
        printf("accept error\n");
        perror("oh!");
        return ;
    }
    handle_recv(conn);
    printf("[RECV] %s\n", conn.recv.c_str());
    conn.send = "I know.";
    handle_send(conn);

};

void M_Listener::run(){
    //开始监听 并设置可以保持10个请求处于等待
    if (-1 == listen(fd, 10)){ 
        printf("listen fd error\n");
        return;
    }
    conn_list.push_back(M_ConnectInfo::create());
    M_ConnectInfo& conn = *conn_list.back();
    socklen_t cli_len = sizeof(conn.addr);//地址结构的长度
    printf("waiting for connect ...\n");
    if (-1 == (conn.fd = accept(fd, 
                    (struct sockaddr*)&conn.addr, 
                    &cli_len)) ){
        //阻塞接收连接(非阻塞应该在之前设置到监听事件)
        printf("accept error\n");
        perror("oh!");
        return ;
    }
    handle_recv(conn);
    printf("[RECV] %s\n", conn.recv.c_str());
    conn.send = "I know.";
    handle_send(conn);

};

void M_SocketBase::handle_recv(M_ConnectInfo& conn){
    //接收内容 
    int recv_len;               //记录接收的长度
    if ((recv_len = recv(conn.fd, conn.buff, BUFF_SIZE, 0)) <= 0 ){ //接收内容进缓冲区
        printf("receive error  %d: %s\n", recv_len, conn.buff);
        return ;
    }
    conn.buff2recv();
};

void M_SocketBase::handle_send(M_ConnectInfo& conn){
    conn.send2buff();
    if(-1 == send(conn.fd, conn.buff, BUFF_SIZE, 0)){ //返回buff内长度为recv_len的内容
        printf("send error\n");
        return ;
    }
};


};
