#include "defer.h"
#include <iostream>

void defer::example(){
    std::cout<<"start test defer"<<std::endl;

    int n = 0;
    for(int i = 1; i<5; i++){
        std::cout<<std::endl<<std::endl;
        std::cout<<"print n:"<<n<<std::endl;
        DEFER(std::cout<<"defer n:"<<n<<std::endl;);
        n+=i;
    }

    std::cout<<"end test defer"<<std::endl;
}
