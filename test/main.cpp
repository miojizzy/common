#include "defer/defer.h"
#include "test/playground.h"
//#include "network/m_epoller.h"
//#include "network/m_socket_base.h"
#include "network/m_select_server.h"
#include <iostream>

int main (int argc, char* argv[]){
//    defer::example();
//    pg::test();
//    common::test();
//
//    common::M_Listener lis;
//    lis.init("127.0.0.1", 8800);
//    lis.run();

    common::M_SelectServer s;
    s.init();
    s.run();
    
    return 0;
}



