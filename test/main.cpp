#include "defer/defer.h"
#include <iostream>

class A{
public:
    A(){};
    ~A(){};
};

class Base{
public:
    A _a;
    Base(){};
    Base(A a):_a(a){std::cout<<"create"<<std::endl;};
    ~Base(){std::cout<<"delete"<<std::endl;};
    Base(Base& b):_a(b._a){std::cout<<"copy create(pass by reference)"<<std::endl;};
    Base(const Base& b):_a(b._a){std::cout<<"copy create(pass by const reference)"<<std::endl;};
    Base(Base&& b):_a(b._a){std::cout<<"copy create(pass by rvalue)"<<std::endl;};
    Base(const Base&& b):_a(b._a){std::cout<<"copy create(pass by const rvalue)"<<std::endl;};
    
    Base& operator=(Base& b){
        std::cout<<"assignment(pass by reference)"<<std::endl;
        if(this == &b) return *this;
        return *this;
    };
    Base& operator=(const Base& b){
        std::cout<<"assignment(pass by const reference)"<<std::endl;
        if(this == &b) return *this;
        return *this;
    };
    Base& operator=(Base&& b){
        std::cout<<"assignment(pass by rvalue)"<<std::endl;
        if(this == &b) return *this;
        return *this;
    };
    Base& operator=(const Base&& b){
        std::cout<<"assignment(pass by const rvalue)"<<std::endl;
        if(this == &b) return *this;
        return *this;
    };

//    Base(Base b):_a(b._a){std::cout<<"copy create(pass by copy)"<<std::endl;};
//    Base(const Base b):_a(b._a){std::cout<<"copy create(pass by const copy)"<<std::endl;};
//    Base& operator=(Base b){
//        if(this == &b) return *this;
//        std::cout<<"assignment(pass by copy)"<<std::endl;
//        return *this;
//    };
//    Base& operator=(const Base b){
//        if(this == &b) return *this;
//        std::cout<<"assignment(pass by const copy)"<<std::endl;
//        return *this;
//    };


};

void test(){
    A a;
    const A ca;
/*
    std::cout<<"\n-----test 1: \n";
    Base b(a);

    std::cout<<"\n-----test 1: \n";
    const Base cb(ca);

    std::cout<<"\n-----test 2: \n";
    Base c(b);

    std::cout<<"\n-----test 2: \n";
    const Base cc(cb);

    std::cout<<"\n-----test 3: \n";
    Base d = b;

    std::cout<<"\n-----test 3: \n";
    const Base cd = cb;

    std::cout<<"\n-----test 4: \n";
    Base e;
    e = b;

    std::cout<<"\n-----test 4: \n";
    const Base ce;
    ce = cb;
*/
    std::cout<<"end"<<std::endl;
};



int main (int argc, char* argv[]){
    defer::example();
    test();
    
    return 0;
}



