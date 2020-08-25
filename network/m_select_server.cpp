#include "network/m_select_server.h"

namespace common{

int M_SelectServer::init(){
   /* 生成socket */
    if (-1 == (listenfd = socket(PF_INET, SOCK_STREAM, 0)) ){//<netinet/in.h>
        printf("create socket error!\n");
        return 0;
    }
 
    /* 生成服务端地址 */
    memset(&ser_addr, 0, sizeof(ser_addr)); // <string.h>       
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //监听的地址范围
    ser_addr.sin_port = htons(PORT);                //监听的端口
 
    /* 设置listenfd为非阻塞 */
    if (!set_fd_nonblock(listenfd)){
        printf( "set listenfd nonblock error\n");
        return 0;
    }
 
    /* 将地址绑定到到 监听事件 */
    if (-1 == bind(listenfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr))){
        printf("bind listenfd and server_addr error!\n");
        return 0;
    }
 
    /* 监听 */
    if (-1 == listen(listenfd, 10)){ //开始监听并设置可以保持10个请求处于等待
        printf("listen fd error\n");
        return 0;
    }
 
    printf("socket init finish!\n");
    return 0;
};

int M_SelectServer::run(){
    while(1){
        printf("curr circle!   %d alive conn   %d socket\n", (int)connfds.size(), 1);
        FD_ZERO(&fds);              //清零要监听的事件的集合
        FD_SET(listenfd, &fds);     //将要监听的fd加入相应的集合
        maxfd = listenfd;           //记录fd的最大值
        for(int i = 0; i< (int)connfds.size(); ++i){
            FD_SET(connfds[i]->fd, &fds);   //添加读事件
            maxfd = std::max(maxfd, connfds[i]->fd);
        }
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        if ( select(maxfd+1, &fds, NULL, NULL, &timeout) < 0){ //对所有要监听的集合进行select 将会修改集合内容只剩激活的fd
            perror("select error!");
            return 0;
        }
        if(FD_ISSET(listenfd, &fds)){
            ConnectInfo* p_conn_info = new ConnectInfo();
            connfds.push_back( p_conn_info);
            handle_conn(connfds.size()-1);
        }
        for(int i = 0; i<(int)connfds.size(); ++i){
            ConnectInfo* p_conn_info = connfds[i];
            if(FD_ISSET(p_conn_info->fd, &fds)){
                handle_recv(i);
            }
            else if(FD_ISSET(p_conn_info->fd, &fds)){
                handle_send(i);
            }
        }
    }
    return 0;
};

int M_SelectServer::handle_conn(int i){
    ConnectInfo *p_conn_info = connfds[i];
    printf("connect fd!\n");
    if(connfds.size() >= FD_LIST_SIZE){
        printf("error! connection full!\n");
        return 0;
    }
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    p_conn_info->fd=accept(listenfd, (struct sockaddr*)&cli_addr, &cli_len);
    if(p_conn_info->fd < 0){
        perror("accept error!");
        return 0;
    }
    set_fd_nonblock(p_conn_info->fd);
    printf("new connection\n");
    return 0;
};

int M_SelectServer::handle_recv(int i){
    ConnectInfo *p_conn_info = connfds[i];
    printf("read fd!\n");
    int len;
    while ((len = recv(p_conn_info->fd, buff, BUFF_SIZE, 0)) > 0 ){
        printf("read once\n");
        if(len <= 0){
            perror("recv error!");
            return 0;
        }
        p_conn_info->recv += std::string(buff).substr(0,len);
    }
    printf("read finish!\n");
    p_conn_info->send = "hahahaha!hello world!";
    return 0;
};

int M_SelectServer::handle_send(int i){
    ConnectInfo *p_conn_info = connfds[i];
    if(p_conn_info->send_offset < (int)p_conn_info->send.size() ){
        int send_size = std::min( (int)p_conn_info->send.size()-p_conn_info->send_offset,  BUFF_SIZE );
        if(-1==send(p_conn_info->fd, 
                    p_conn_info->send.substr(p_conn_info->send_offset, send_size).c_str(),
                    send_size,
                    0) ){
            perror("send error!");
            return 0;
        }
        p_conn_info->send_offset += send_size;
    }
    if(p_conn_info->send_offset >= (int)p_conn_info->send.size() ){
        connfds.erase(connfds.begin()+i);
        delete p_conn_info;
        return 0;
    }
    return 0;
};


};
