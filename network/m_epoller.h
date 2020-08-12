#ifndef __H_M_EPOLL__
#define __H_M_EPOLL__

#include <sys/epoll.h>
#include <unistd.h>
#include "network/m_event.h"

#define MAX_EVENT_NUM 10

namespace common{

/*
 *  封装一层event
 *  绑定了fd 和 ev
 *
 * */
class M_EventInfo: public M_Event{
    public:
        M_EventInfo(){};
        virtual ~M_EventInfo(){};

        struct epoll_event* ev_ptr;
};

/*
 * 管理epfd
 * event的增删修改和响应
 *
 * */
class M_EpollerBase{
    public:

        M_EpollerBase():_epfd(-1),_ep_size(0),_ep_list(NULL){};
        ~M_EpollerBase(){
            close(_epfd);
        };

        bool create(int size);

        void add(M_EventInfo* ptr, int op);
        void mod(M_EventInfo* ptr, int op);
        void del(M_EventInfo* ptr);

        void wait();

        virtual void run() = 0;

    protected:
        int _epfd;

        // wait 得到的待处理event 和 长度
        int _ep_size;
        struct epoll_event* _ep_list;
};


}; //common

#endif //__H_M_EPOLL__
