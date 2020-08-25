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
class M_EventInfo: public virtual M_Event{
    public:
        M_EventInfo(){};
        virtual ~M_EventInfo(){close(fd); };
        struct epoll_event* ev_ptr;
};


/*
 * 用于管理epfd 以及绑定其上的所有fd
 *
 * */
class M_EpollerBase{
    protected:
        int _epfd;      //管理的epfd
        int _ep_size;   //每次wait得到的数量
        struct epoll_event* _ep_list;

    public:
        M_EpollerBase():_epfd(-1),_ep_size(0),_ep_list(NULL){};
        ~M_EpollerBase(){
            close(_epfd);
        };


        /*  初始化epfd，设定管理fd数量上线。
            数量会比设定上限大一个做保留，epfd小等于零则会返回false*/
        bool create(int size);


        /* 对管理的fd增删修改  要配合eventinfo类使用
            默认给所有的操作增加 ET */
        void add(M_EventInfo* ptr, int op);
        void mod(M_EventInfo* ptr, int op);
        void del(M_EventInfo* ptr);


        /*
         *  调用wait获取当前时刻的活跃事件
         * */
        void wait();


        /*
         *  处理活跃事件列表  要自己实现
         * */
        virtual void process() = 0;

};


}; //common

#endif //__H_M_EPOLL__
