#include "test/playground.h"
#include <iostream>

void pg::test(){
    A a;
    const A ca;

    std::cout<<"\n-----test 1: \n";
    Base b(a);

    std::cout<<"\n-----test 1: \n";
    const Base cb(ca);

    std::cout<<"\n-----test 2: \n";
    Base c(b);

    std::cout<<"\n-----test 2: \n";
    const Base cc(cb);

    std::cout<<"\n-----test 3: \n";
    Base d;

    std::cout<<"\n-----test 3: \n";
    //const Base cd = cb;

    std::cout<<"\n-----test 4: \n";
    Base e;
    e = b;

    std::cout<<"\n-----test 4: \n";
    const Base ce;
    //ce = cb;

    std::cout<<"end"<<std::endl;
};
