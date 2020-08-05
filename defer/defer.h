#ifndef __H_DEFER__
#define __H_DEFER__

#include <functional>
#include <iostream>

/* 
 *
 *  DEFER(func());
 *
 *  DEFER({
 *      func1();
 *      func2();
 *  }))
 *
 *
 * 类似于go的defer  为了在推出作用域时候执行一段代码使用
 * 使用临时变量绑定std::function到销毁函数上来实现 
 *
 *  **注意** 因为同一作用域临时变量释放顺序不定，因此不可以依赖任何临时变量
 *
 * */


/**********************************************************************/
#define _GET_DEFER_NAME_BODY(x,y) x##y //临时变量名的拼接，使用__COUNTER__ 区分
#define _GET_DEFER_NAME(x,y) _GET_DEFER_NAME_BODY(x,y)
#define DEFER(expr)   defer::Defer<std::function<void()>> _GET_DEFER_NAME(__defer,__COUNTER__)([&]()->void{expr;});

namespace defer{

template<typename T>
class Defer{
public:
    Defer(T&& t):_t(std::move(t)){}; //这里关于std::function怎么传还有点疑问m
    ~Defer(){_t();};
private:
    T _t;
};


void example();

};//nsp defer


#endif //__H_DEFER__
