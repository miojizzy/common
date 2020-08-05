 
# Makefile 基础学习笔记

makefile 有三部分组成，一部分是变量，一部分是依赖关系，一部分是执行命令

```
<变量1> = <值>
<依赖者>: <被依赖者1> <被依赖者2>
    <命令1>
    <命令2>
```


## 变量
变量一般为全大写，使用的时候用 $(xxx) 的方式
有多种赋值方式

- =赋值：表示使用<值>进行赋值，若<值>中有使用变量2，则变量2用在makefile展开完成后的最终的值来替代
- :=赋值：表示使用<值>进行赋值，若<值>中有使用变量2，则变量2用当下时刻的值来替代
- ?=赋值：表示使用<值>进行赋值，若变量不存在则进行复制，否则不变
- +=赋值：表示使用<值>进行append赋值，若变量不存在则创建，存在的话则继承创建时候的赋值类型(=或:=)

**注意变量赋值结尾不能有空格，尤其是加注释时候的空格tab等**


## 依赖关系
依赖关系一般顶头开始写起，由冒号分割，例如：
被依赖者可以为具体文件(相对于makefile来说的路径要对), 或者为某个依赖者，而这个依赖
者又会依赖其他东西，从而形成一个依赖关系的有向无环图
(然后编译从没有依赖的点开始，执行依赖者下相应的命令)


## 执行命令
命令一般为一级tab后写起，紧跟在依赖关系下，为执行shell的命令
一般都为gcc echo mkdir rm 等
**注意依赖关系的空间与命令的空间不相同**
```
.
├── server
│   ├── server.cpp
│   └── server.h
├── main.cpp
└── makefile
例如使用"gcc -I./ -I./server ..."的话，main中可以直接包含server.h而不用加路径，
但是在依赖关系中还是需要"main.o: main.cpp server/server.h" 加上路径，
而VPATH变量就是指定依赖关系的搜索路径，在这里面加入了"./server"后就不用再在依赖关系中加路径了
因为一些失败的设计，可能会导致不同路径下的头文件名可能会产生重复(而头文件内容不同)，因此不推荐使用VPATH
```



--------------------------------------------------------------------------------

# 进阶

## 函数
```
# 正则匹配后枚举 以空格分割字符串
$(wildcard ./*.cpp) 

# 以空格分割 循环遍历字符串的字串
$(foreach n, $(STRING), $(n)_new ) #遍历$(STRING)为n，以新的格式$(n)_new返回

# 正则匹配替换
$(patsubst %.cpp, %.o, $(CPPS)) #正则匹配$(CPPS)中的.cpp并换成.o 返回
$(CPPS:.cpp=.o) $(CPPS:%.cpp=%.o) #这两种也可以实现相同功能

# 执行shell命令 并将其输出作为返回
$(shell pwd) 

# 某文件 去掉路径 或 取路径
$(dir $(FILE))
$(nodir $(FILE))
```


## 依赖关系规则
例如:
%.o: %.cpp
表示所有的.o文件都要依赖同名.cpp 在下面跟的gcc命令应该就是使用这个.cpp编译出这个.o
这里有几个新变量:
- $@: 表示当前依赖关系中的依赖者
- $<: 表示当前依赖关系中的第一个被依赖者
(暂时用这两个就够了)
因此这个规则的写法是
```
%.o: %.c
    gcc -o $@ -c $<
```

## 依赖关系规则进阶
因为在工程中每一个cpp会引入很多的头文件，因此在依赖关系中，.o文件不只是依赖同名的.cpp，还要依赖很多的.h
为了自动生成这种复杂的依赖关系，使用的 gcc -MM xxxx.cpp 的功能来生成依赖关系并输入到文件中，
再在makefile中引入这个文件，一般命名为同名.d文件，这样就有自动生成依赖关系的功能
**要注意的点就是路径问题，不好调试**
一个简单例子:
```
CPPS := $(wildcard *.cpp)
DEPS := $(CPPS:.cpp=.d)
# 给每一个cpp生成一个.o一个.d  分别取三者的集合
%.d: %.cpp
    gcc -MM $< > $@
# 生成好了之后引入进来
-include $(DEPS)

```




