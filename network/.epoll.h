// MyEpoll/MyEpoll.h
#ifndef __H_EPOLL__
#define __H_EPOLL__
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include<string>
#include<vector>


#define MAX_EVENT 20
#define READ_BUF_LEN 10
using namespace std;

namespace NetWork{

//const int MAX_EVENT = 20;
//const int READ_BUFF_SIZE = 10;

class ConnectInfo
{
    public:
        std::string req;        // 请求数据
        std::string resp;       // 返回数据

        ConnectInfo(int param_fd):req(""), resp(""), 
                _recv_offset(0), _send_offset(0), _fd(param_fd), _ori_url(""){};
        virtual ~ConnectInfo(){
            close(_fd);
        };

        //是否接收完(自己定义请求的头部数据来判断是否结束)
        bool isRecvFinish(){return true;};

    private:
        int _recv_offset;       //记录已接收位置
        int _send_offset;       //记录已发送位置
        int _fd;                //该连接的conn_fd描述符
        std::string _ori_url;   //请求原始完整串
};



class MySocketListener
{
    public:
        MySocketListener(){};
        ~MySocketListener(){};

        // 初始化，指定ip端口和同时处理并发的数量(httpworker)
        bool init(const char* ip, const int& port, const int& num);
        // 开始监听
        bool start();

        bool _handle_accept(int fd);    //处理链接事件
        bool _handle_send(int i);       //处理发送事件
        bool _handle_recv(int i);       //处理接收事件
        bool _dosth(int i);             //调用处理函数入口
        int _setnonblock(int fd);       //设置fd为非阻塞

        MyThreadPool* _pool;    //http_worker线程池
        int _epfd;              //epoll实例
        int _listenfd;          //监听socket的fd
        struct epoll_event _listenev;   //监听socket的事件
        struct epoll_event* _event_list;//处理事件的list
        struct sockaddr_in _server_addr;//监听的地址
};

class MyHttpWorker : public Worker
{
    public:
        MyHttpWorker(){};
        ~MyHttpWorker(){};
        void run();
        ConnectInfo *ev_ptr;
        MySocketListener* listener_ptr;
};



};

#endif // __H_EPOLL__
