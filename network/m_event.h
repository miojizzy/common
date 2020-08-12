#ifndef __H_M_EVENT__
#define __H_M_EVENT__

namespace common{

class M_Event{
    public:
        M_Event(){};
        virtual ~M_Event(){};

        int fd;
};

/* 设置fd为非阻塞 */
bool set_fd_nonblock(int fd);


};
#endif
