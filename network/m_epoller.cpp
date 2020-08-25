#include "network/m_epoller.h"

namespace common{


bool M_EpollerBase::create(int size){
    if( (_epfd = epoll_create(size+1)) <= 0 ) return false;
    return true;
}

void M_EpollerBase::add(M_EventInfo* ptr, int op){
    ptr->ev_ptr->events |= EPOLLET | op;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, ptr->fd, ptr->ev_ptr);
}

void M_EpollerBase::mod(M_EventInfo* ptr, int op){
    ptr->ev_ptr->events |= EPOLLET | op;
    epoll_ctl(_epfd, EPOLL_CTL_MOD, ptr->fd, ptr->ev_ptr);
}

void M_EpollerBase::del(M_EventInfo* ptr){
    epoll_ctl(_epfd, EPOLL_CTL_DEL, ptr->fd, NULL);
}
void M_EpollerBase::wait(){
    _ep_size = epoll_wait(_epfd, _ep_list, MAX_EVENT_NUM, -1);
    if(_ep_size <= 0) return;
    this->process();
}

void test(){
   return; 
}

}; //common
