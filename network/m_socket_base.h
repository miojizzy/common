#ifndef __H_M_SOCKET_BASE_
#define __H_M_SOCKET_BASE_

#include <string>
#include <vector>
#include <netinet/in.h> //struct sockaddr_in
#include <math.h>//std::min
#include <unistd.h>//close
#include <string.h> //strncpy()
#include <memory> //unique_ptr
#include "network/m_event.h"

namespace common{

#define BUFF_SIZE 1024

class M_ConnectInfo;
typedef std::unique_ptr<M_ConnectInfo> M_ConnectInfoPtr;

/*
 *  socket 连接的信息，并绑定了相应的连接fd
 * */
class M_ConnectInfo : virtual public M_Event{
    private:
        M_ConnectInfo():recv(""),send(""),recv_offset(0),send_offset(0){
            memset(&addr, 0, sizeof(addr));
            memset(buff, 0, sizeof(char)*BUFF_SIZE);
        };
    public:
        static M_ConnectInfoPtr create() {return M_ConnectInfoPtr(new M_ConnectInfo()); };
        ~M_ConnectInfo() {close(fd); };

        std::string recv;
        std::string send;
        int recv_offset;
        int send_offset;
        char buff[BUFF_SIZE];
        struct sockaddr_in addr;
        int addr_size;

        inline void buff2recv(){
            std::string tmp = buff;
            recv+=tmp;
        };

        inline void send2buff(){
            int len = std::min(BUFF_SIZE, (int)send.size()-send_offset);
            strncpy(buff, send.c_str()+send_offset, len);
            send_offset += len;
        };
};


/*
 *  socket base
 *  可以通过参数配置是否为阻塞监听
 * */
class M_SocketBase{
    public:
        M_SocketBase():fd(-1) {memset(&addr, 0, sizeof(addr)); };
        virtual ~M_SocketBase() {close(fd); };

        int fd;     //监听fd
        struct sockaddr_in addr;    //本机地址
        std::vector<M_ConnectInfoPtr> conn_list; //管理所有存活的连接信息

        /* 指定ip port 和 是否阻塞 快速初始化
            client ip 为要连接的地址
            server ip 为要监听的地址范围 0.0.0.0全选 或 127.0.0.1自己 */
        bool init(const char* ip, int port, bool nonblock=false);

        /* 根据具体 客户端 和 服务器 实现不同内容
         * */
        virtual void run() = 0;

        /* 用于处理 接收连接 读 写 等函数
         * */
        void handle_accept(M_ConnectInfo& conn);
        void handle_recv(M_ConnectInfo& conn);
        void handle_send(M_ConnectInfo& conn);

};

class M_Listener: public M_SocketBase{
    public:
        ~M_Listener() {close(fd); };

        void run();

};


};


#endif //__H_M_SOCKET_BASE_
