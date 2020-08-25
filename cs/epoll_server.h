#ifndef __H_M_EPOLL_SERVER__
#define __H_M_EPOLL_SERVER__

namespace common{

class M_EpollServer: public M_SocketBase, M_EpollerBase{
    public:
        M_EpollServer(){};
        ~M_EpollServer(){};

        bool init(const char* ip, int port, bool nonblock){
            M_SocketBase::init(ip, port, nonblock);
            create(10);
            set_fd_nonlock(fd);
            add(fd)
        };


};


};

#endif //__H_M_EPOLL_SERVER__
