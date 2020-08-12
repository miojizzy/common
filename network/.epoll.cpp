// MyEpoll/MyEpoll.cpp
#include "MyEpoll.h"

int MySocketListener::_setnonblock(int fd)
{
    // 获取当前flag
    int flags;
    flags = fcntl(fd, F_GETFL, 0);//获取fd当前的状态
    if (-1 == flags)
    {
        printf("[ERROR] get fd flag error");
        return -1;
    }
    // 设置flag
    if( fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)//设置增加非阻塞
    {
        printf("[ERROR] set fd flag error");
        return -1;
    }
    return 0;
}
bool MySocketListener::init(const char* ip, const int& port, const int& num)
{
    // 设置ip和端口
    _server_addr = { 0 };
    _server_addr.sin_family = AF_INET;  //指定协议
    inet_aton (ip, &(_server_addr.sin_addr));   //字符串ip转换为网络格式
    _server_addr.sin_port = htons(port);        //数字端口号转换为网络格式

    // socket相关设置
    _listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == _listenfd) {
        //perror("Open listen socket");
        return -1;
    } // 打开 socket 端口复用, 防止测试的时候出现 Address already in use
    int on = 1;
    if(-1 == setsockopt( _listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) )){
        printf("[ERROR]set socket opt error\n");
        return 0;
    } // 设置不阻塞
    if(-1 == _setnonblock(_listenfd)){
        printf("[ERROR]set socket nonblock error\n");
        return 0;
    } // 绑定
    if( -1 == bind(_listenfd, (const struct sockaddr *)&_server_addr, sizeof (_server_addr))){
        printf("[ERROR]bind error\n");
        return 0;
    } // 监听
    if( -1 == listen(_listenfd, 200)){
        printf("[ERROR]listen error\n");
        return 0;
    }

    // 开启httpworker的线程池
    _pool = new MyThreadPool();
    _pool->open(num);

    // 创建epoll实例
    _epfd = epoll_create(1);
    if (1 == _epfd) {
        //perror("Create epoll instance");
        return 0;
    }
    //创建事件
    _event_list = new epoll_event[MAX_EVENT];
    MyEvent* p_ = new MyEvent(_listenfd);

    _listenev.data.ptr = p_;
//  _listenev.data.fd = _listenfd; //data为unoin不能同时设置两个
    _listenev.events = EPOLLIN | EPOLLET /* 边缘触发选项。 */;
    // 添加epoll的事件
    if(-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, _listenfd, &_listenev)) {
        //perror("Set epoll_ctl");
        return 0;
    }
    return true;
}
bool MySocketListener::start()
{
    printf("Start Listen ...\n");
    for (;;) {
        int event_count;
        // 等待事件
        event_count = epoll_wait(_epfd, _event_list, MAX_EVENT, -1);
        for (int i = 0 ; i < event_count; i++) {
            uint32_t events = _event_list[i].events;
            MyEvent* ev_ptr = (MyEvent*)_event_list[i].data.ptr;
            // 判断epoll是否发生错误
            if ( events & EPOLLERR || events & EPOLLHUP || (! events & EPOLLIN)) {
                printf("[ERROR]Epoll has error\n");
                delete _event_list[i].data.ptr;
                continue;
            }
            // 分类处理事件
            if (_listenfd == ev_ptr->_fd && !(events&EPOLLONESHOT)) {//链接事件
                printf("FOR accept things\n");
                int accp_fd = 0;
                while( (accp_fd=accept(_listenfd, NULL, NULL)) > 0 )
                    _handle_accept(accp_fd);
            } else if (events & EPOLLOUT){  //发送数据事件
                printf("FOR send things\n");
                _handle_send(i);
            } else if (events & EPOLLIN){   //接收数据事件
                printf("FOR recv things\n");
                _handle_recv(i);
            }
        }
    }
    close (_epfd);
    return true;
}
bool MySocketListener::_handle_accept(int fd)
{
    if(-1 == _setnonblock(fd)){//设置非阻塞
        printf("[ERROR]set socket nonblock error\n");
        return false;
    }
    // 设置读数据事件
    struct epoll_event ev;
    //ev.data.fd = fd; //这里设置自定义指针就不能设置fd，fd保存在自定类型里
    MyEvent *ev_ptr = new MyEvent(fd); //为当前链接描述符创建新的交互数据的事件
    ev.data.ptr = ev_ptr;
    ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    // 为新accept的 file describe 设置epoll事件
    if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev)) {
        printf("[ERROR]epoll_ctl error\n");
    }
    return true;
}
bool MySocketListener::_handle_recv(int i)
{
    struct epoll_event& enevt = _event_list[i];
    MyEvent *ev_ptr = (MyEvent*)enevt.data.ptr;
    ssize_t result_len = 0;
    char buf[READ_BUF_LEN + 1] = { 0  };
    while( (result_len = read(ev_ptr->_fd, buf, READ_BUF_LEN)) > 0 ) //要循环读到没有
    {
        buf[result_len] = '\0';
        ev_ptr->_recv_offset += result_len;
        ev_ptr->_ori_url += buf;
    }
    if (-1 == result_len) { //读取出错误
        if (EAGAIN != errno) {
            printf("[ERROR] read error\n");
            perror("read error");
        }
        //读完，更新event (oneshot)
        epoll_ctl(_epfd, EPOLL_CTL_ADD, ev_ptr->_fd, &enevt);
        if(ev_ptr->isRecvFinish()) _dosth(i);
    }
    return true;
}
bool MySocketListener::_handle_send(int i)
{
    struct epoll_event& enevt = _event_list[i];
    MyEvent *ev_ptr = (MyEvent*)enevt.data.ptr;
    int result_len = 0;
    if(ev_ptr->_send_offset == 0)
    {
        size_t len = ev_ptr->resp.length();
        char* buff = new char[len+200];
        sprintf(buff,"HTTP/1.1 200 OK\r\nContent-Type: application/json;charset=utf-8\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n%s",len,ev_ptr->resp.c_str());
        ev_ptr->resp = buff;
    }
    while(ev_ptr->resp.length() > ev_ptr->_send_offset)
    {
        const char* buf = ev_ptr->resp.c_str() + ev_ptr->_send_offset;
        result_len = write(ev_ptr->_fd, buf, READ_BUF_LEN);
        ev_ptr->_send_offset += result_len;
        if(result_len <= 0) break;
    }
    if (-1 == result_len) { //读取出错误
        if (EAGAIN != errno) { //读完，更新event (oneshot)
            epoll_ctl(_epfd, EPOLL_CTL_ADD, ev_ptr->_fd, &enevt);
        }
    }
    if (ev_ptr->resp.length() == ev_ptr->_send_offset)
    {
        epoll_ctl(_epfd, EPOLL_CTL_DEL, ev_ptr->_fd, NULL);
        close(ev_ptr->_fd);
        delete ev_ptr;
    }
    return true;
}

bool MySocketListener::_dosth(int i)
{
    //接收完后交给worker处理
    struct epoll_event& event = _event_list[i];
    MyHttpWorker* worker = new MyHttpWorker();
    worker->ev_ptr = (MyEvent*)event.data.ptr;
    worker->listener_ptr = this;
    _pool->addTask(worker);
}
void MyHttpWorker::run()
{
    // 添加处理逻辑
    ev_ptr->resp = ev_ptr->_ori_url;
    ev_ptr->resp += "11111111111111";
    printf("do le songthing!\n");

    //处理逻辑完成后，事件改为返回数据事件
    struct epoll_event ev;
    ev.data.ptr = ev_ptr;
    ev.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
    if (-1 == epoll_ctl(listener_ptr->_epfd, EPOLL_CTL_MOD, ev_ptr->_fd, &ev)) {
        printf("[ERROR]epoll_ctl error\n");
    }
}

