
CC			= gcc
CXX			= g++
# 使编译器支持c++11
CFLAGS		:=
CXXFLAGS	:= -std=c++11 -g -fPIC -Wall -fno-elide-constructors
INCLUDE		:= -I./

# 指定代码子文件夹
# 指定当前文件夹
# 指定中间产物文件夹
# 指定二进制文件的文件夹
SUB_DIR 	= ./test ./defer ./network
WORKSPACE 	= $(shell pwd)
OBJ_DIR 	= $(WORKSPACE)/build
BIN_DIR 	= $(WORKSPACE)/bin

# 遍历所有子文件夹并得到所有cpp文件
# 根据所有cpp改名得到全部的.o文件名
# 根据所有cpp改名得到全部的.d依赖文件并指定了依赖的路径
CPPS 	= $(foreach n, $(SUB_DIR), $(wildcard $(n)/*.cpp ) )
OBJS 	= $(patsubst %.cpp, %.o, $(CPPS) )
DEPS 	= $(patsubst %.cpp, $(OBJ_DIR)/%.d, $(CPPS) )

# 指定makefile文件在展开过程中 搜索头文件的路径
# 在这里指定了子目录了之后，在书写依赖关系的时候就不需要写相对路径
#VPATH := ./main:./common #但是这里不用

#--------------------------------------------------------------------------------

# 这里指定某些在依赖关系中的项“不为文件,而为指令”
# all紧跟在.PHONY后面 默认make的就是all

# 全局目标all 依赖要生成的所有可执行文件 
.PHONY: all
all: tar2 #tar1

# 目标1
tar: $(OBJS)
	g++ $(CFLAGS) -o tar $(OBJS) $(INCLUDE) 


# 目标2 可执行文件
# 目标2依赖所有obj, 在command中-o连接起来所有的.o文件, 生成最终的可执行文件
# $(CC)或$(CXX)				指定gcc/g++编译器
# $(CFLAGS)或$(CXXFLAGS)	指定编译参数
# $(INCLUDE)				指定引用头文件时候的“当前目录”
# $(BIN_DIR)/$@				指定了输出的可执行文件名为“依赖者”，并指定了路径
# $(foreach n, $(OBJS), $(OBJ_DIR)/$(n)
# 指定了要连接的所有obj文件，并遍历且增加了.o文件的路径
#
tar2:$(OBJS)  
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(BIN_DIR)/$@ $(foreach n, $(OBJS), $(OBJ_DIR)/$(n) )

#--------------------------------------------------------------------------------

# 规则1
# 生成依赖关系
# 使用 gcc -MM 的方式生成.o文件对cpp与h的依赖关系到文件中，然后直接在makefile中include进来
# 但是有一个缺点是.o文件不带路径，需要使用 $< “被依赖者的第一个”(带路径,通常都为.o文件的同名cpp)进行替换修改
# 得到自动生成的依赖关系中的“依赖者”，然后调用gcc -MM 生成依赖关系并替换掉“依赖者”添加进依赖关系中
# 最后一行是在build文件中建立合法目录路径，并将.d文件移到其中。
# (因为.d文件是依赖关系，只在makefile中include用，所以对编译过程没有影响)
%.d: %.cpp
	echo -n $< | sed 's/\.cpp/\.o/' > $@; \
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MM $< | sed 's/.*:/:/' >> $@; \
	mkdir -p  $(OBJ_DIR)/$(dir $@) && mv $@ $(OBJ_DIR)/$@

# 在这里include进来所有依赖文件 前面加-号表示include错误时跳过
-include $(DEPS)

# 规则2
# 编译出.o文件
# 对于每一个cpp文件都有对应的.o文件，使用 gcc -o -c 来生成
# 生成的时候指定了生成的obj所在的文件夹(生成之前先创建路径)
%.o: %.cpp
	mkdir -p  $(OBJ_DIR)/$(dir $@); \
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(OBJ_DIR)/$@ -c $< 


#--------------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf build/* 
	rm -rf bin/* 
