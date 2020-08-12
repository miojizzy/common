# ifndef __H_PLAYGROUND__
# define __H_PLAYGROUND__

#include <iostream>

namespace pg{


class A{
public:
    A(){};
    ~A(){};
};

class Base{
public:
    A _a;
    explicit Base(){std::cout<<"blank create"<<std::endl;};
    explicit Base(A a):_a(a){std::cout<<"create"<<std::endl;};
    ~Base(){std::cout<<"delete"<<std::endl;};
    explicit Base(Base& b):_a(b._a){std::cout<<"copy create(pass by reference)"<<std::endl;};
    explicit Base(const Base& b):_a(b._a){std::cout<<"copy create(pass by const reference)"<<std::endl;};
    explicit Base(Base&& b):_a(b._a){std::cout<<"copy create(pass by rvalue)"<<std::endl;};
    explicit Base(const Base&& b):_a(b._a){std::cout<<"copy create(pass by const rvalue)"<<std::endl;};
    
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

void test();

};
# endif //__H_PLAYGROUND__
