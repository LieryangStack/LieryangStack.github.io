# 第二层: 变量   
#         = (替换)     += (追加)    :=(恒等于，不可变的)
#         使用变量  $(变量名)
# 第三层: 隐含规则
#         %.c %.o 任意的 .c或者.o  
#         *.c *.o 所有的 .c .o
# 第四层: 通配符
#         $^所有的依赖文件  $@所有的目标文件 $<所有的依赖文件的第一个文件
# 第五层:

# 如果没有参数给make命令，默认目标会被执行
default_target: all

.PHONY : default_target

TARGET := sample2

CC := gcc

# 当前目录下所有的c语言文件
SRCS:= $(wildcard *.c)

INCS:= $(wildcard *.h)

# 获取当前 Makefile 文件的完整路径
CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

# 获取当前 Makefile 文件所在的目录
INCS_PATH := -I$(dir $(CURRENT_MAKEFILE))/includes/

# 把SRCS中的每个文件都由.c替换成.o
OBJS:= $(SRCS:%.c=%.o)

all: $(TARGET)
	@echo "编译结束"

$(TARGET):$(OBJS)
	@echo "test" $(SUFFIXES)
	$(CC) $^ -o $@
# 多个文件同时编译的时候不能即用 -o 又用 -c
%.o:%.c main.h
	@echo "开始编译" $<
	@echo $(SRCS)
	@echo $(OBJS)
	$(CC) -c $< -o $@ -I./includes
main.h : 
	@echo "头文件"

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
