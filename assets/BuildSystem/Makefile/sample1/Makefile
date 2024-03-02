#是注释
#第一层：显式规则
#目标文件：依赖文件
#[TAB]指令
#
# 第一个目标文件是我的最终目标
#
# rm -rf hello.o hello.s hello.i hello
# 伪目标 .PHONY:
.PHONY:hello
hello:hello.o
	gcc hello.o -o hello
hello.o:hello.s
	gcc -c hello.s -o hello.o
hello.s:hello.i
	gcc -S hello.i -o hello.s
hello.i:hello.c
	gcc -E hello.c -o hello.i

.PHONY: clear
.PHONY: clearall

clearall:
	rm -rf hello hello.o hello.s hello.i
clear:
	rm -rf hello