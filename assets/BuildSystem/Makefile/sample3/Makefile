# 第二层: 变量   
#         = (替换)     += (追加)    :=(恒等于，不可变的)
#         使用变量  $(变量名)
# 第三层: 隐含规则
#         %.c %.o 任意的 .c或者.o  
#         *.c *.o 所有的 .c .o
# 第四层: 通配符
#         $^所有的依赖文件  $@所有的目标文件 $<所有的依赖文件的第一个文件
# 第五层:


# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target


TARGET := test

CC := gcc

# 当前目录下所有的c语言文件
SRCS:= $(wildcard *.c)

INCS:= $(wildcard *.h)

# 把SRCS中的每个文件都由.c替换成.o
OBJS:= $(SRCS:%.c=%.o)

INC = -I/home/lieryang/Desktop/makefile-study/2.mutli-file/

all : $(TARGET)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@
# 多个文件同时编译的时候不能即用 -o 又用 -c
%.o:%.c
	@echo "开始编译"
	@echo $(SRCS)
	@echo $(OBJS)
	$(CC) -c $^ -o $@ $(INC)
	@echo "编译结束"

.PHONY : all


.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
