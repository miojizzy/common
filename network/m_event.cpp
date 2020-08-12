#include "network/m_event.h"

#include <fcntl.h>
#include <stdio.h>

namespace common{


/* 设置fd为非阻塞 */
bool set_fd_nonblock(int fd){
    int opt;
    if( (opt = fcntl(fd, F_GETFL)) < 0){
        perror("fcntl(fd, F_GETFL) error!");
        return false;
    }
    if( (fcntl(fd, F_SETFL, opt|O_NONBLOCK)) < 0){
        perror("fcntl(fd, F_SETFL, opt) error!");
        return false;
    }
    return true;
}


};

