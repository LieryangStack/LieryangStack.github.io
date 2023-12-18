# gcc使用总结
先回顾gcc编译相关的知识，是因为Makefile文件是通过编译命令进行编译文件（比如：gcc）
## 1 基本概念
&emsp;&emsp;gcc是linux系统集成的编译器，gcc是可以编译C++程序的，gcc原名GNU C Compiler，最初是C语言的编译器，但是经过发展之后，它变成了一个可以支持C++、Fortran、Pascal、Objective-C、Java、Ada，以及Go与其他语言编译的编译器套件，其名称也因此改为了GNU Compiler Collection。g++便是其中的一部分，用于处理c++语言。虽然大多数情况下，我们直接使用g++命令来编译c++程序，但直接使用gcc命令也可以编译C++程序的，当然前提是安装了g++（gcc-c++）模块。gcc命令会根据源程序的后缀名来决定实际使用的编译器，编译过程与直接使用g++完全一样，但是，链接过程有点不同。`g++命令会自动给你加上c++标准库的链接，但gcc命令却不会给你自动加上，因些需要手动加上`。
&emsp;&emsp;在linux环境下编辑程序，首先需要克服的便是没有集成开发环境的一键式操作所带来的麻烦。这其中涉及命令行操作、编译选项的设定、文件依赖关系的书写（makefile）等问题。（若编译C++文件，则只需将下列命令的gcc换为g++，源文件的后缀应为 .c/.cpp/.c++/.cc等）。

## 2 GCC编译过程

![请添加图片描述](https://img-blog.csdnimg.cn/6527f210287b4abab3dbc83b7070326a.png?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBA5p2O5bCU6Ziz,size_20,color_FFFFFF,t_70,g_se,x_16)


```bash
gcc -E test.c -o test.i # 预处理
gcc -S test.i -o test.s # 汇编
gcc -c test.s -o test.o # 编译
gcc test.o -o test      # 链接
```

### 2.1 不同系统后缀文件代表的含义

 - **windows**中静态库是以 .lib 为后缀的文件，动态库是以 .dll 为后缀的文件，编译中间文件是以 .obj 为后缀的文件。
 - **linux**中静态库是以 .a 为后缀的文件，动态库是以 .so为后缀的文件， 编译中间文件是以 .o 为后缀的文件。
 - **mac**中静态库是以.a为后缀的文件，动态库是以.dylib为后缀的文件，以.framework为后缀的文件可能是静态库，也可能是动态库。

### 2.2 重点区分编译和链接
### 2.2.1 编译

 <span style="background-color: lightcoral; font-size: 20px;">编译过程只会**检查**变量或者函数有没有声明，**不会检查**变量或者函数是否定义
 </span>

 <span style="background-color: lightcoral; font-size: 20px;">**编译检查**的是函数的声明，**链接检查**的是函数的定义
 </span>

```c
/* filename: main.cpp */
// int xxx();
 
int main()
{
    xxx();
}
```

编译一下，自然编译不过， 信息为：

```bash
ubuntu@VM-0-15-ubuntu:~/taoge/cpp/test$ g++ -c main.cpp 
main.cpp: In function ‘int main()’:
main.cpp:5:9: error: ‘xxx’ was not declared in this scope
     xxx();
         ^
ubuntu@VM-0-15-ubuntu:~/taoge/cpp/test$ 
```

打开上述的注释， 得到：

```c
int xxx();
 
int main()
{
    xxx();
}
```

编译一下， 能通过吗？ 看看：

```c
/* 可以编译通过 */
ubuntu@VM-0-15-ubuntu:~/taoge/cpp/test$ g++ -c main.cpp     
ubuntu@VM-0-15-ubuntu:~/taoge/cpp/test$ 
```

### 2.2.2 链接

 链接一下， 发现xxx()未定义。

```c
ubuntu@VM-0-15-ubuntu:~/taoge/cpp/test$ g++ main.o
main.o: In function `main':
main.cpp:(.text+0x5): undefined reference to `xxx()'
collect2: error: ld returned 1 exit status
```

要区分编译和链接所做的事。

再仔细看看上述的英文提示， 编译不通过， 显示的是not declared,  而链接不通过， 显示的是undefined.  所以， 一目了然了。


## 3 常用参数
### 3.1 Gcc编译过程基本参数总结
```c
-E /* 只执行预处理操作，内容直接输出到屏幕（可以通过 -o 命令输出到指定文件中） */
-S /* 只执行到编译操作完成，不进行汇编操作，生成的是汇编文件(.s或.asm)，内容为汇编语言 */
-c /* 执行编译和汇编，但不进行链接，既只生成可重定位目标文件(.o)，为二进制文件，不生成完整的可执行文件 */
-o filename /*将操作后的内容输出到filename指定的文件中*/
-v /*查看头文件的搜索目录*/
```
### 3.2 创建静态库
&emsp;&emsp;Linux静态库命名规范，必须是`lib[your_library_name].a`，lib为前缀，中间是静态库名，扩展名为.a。编译静态库一共有两个步骤。

 1. 生成目标文件 staticmath.o 
 2. 使用ar工具将目标文件打包成.a静态库文件。

```c
gcc -c staticmath.c /*生成目标文件 staticmath.o*/
/* -r 表示将后面的文件列表添加到文件包，如果文件包不存在就创建它，如果文件包中已有同名文件就替换成新的
 1. -s 是专用于生成静态库的，表示为静态库创建索引，这个索引被链接器使用。
 */
ar -crv libstaticmath.a staticmath.o /*使用ar工具将目标文件打包成.a静态库文件*/
```
### 3.3 创建动态库（共享库）
 1. 生成目标文件，此时要加编译器选项 -fPIC
 2. 生成动态库

```c
gcc -c -fPIC dmath.c
gcc dmath.o -shared -o libdmath.so
/* 上面两步可以合为一步 */
gcc dmath.c -fPIC -shared -o libdmath.so
```

### 3.4 链接库
&emsp;&emsp;特别要主要的是，编译器会首先找libstaticmath.so，如果有就链接它，如果没有就找有没有静态库libstaticmath.a。编译器是优先考虑共享库的，如果希望编译器只链接静态库，可以使用 `-static`选项。
```bash
gcc main.c -L. -static -lstaticmath -o main 
```
### 3.5 gcc链接库参数总结
```bash
-L           /*告诉编译器去哪里找需要的库文件*/
-L.          /*表示在当前目录找*/
-I           /*告诉编译器去哪里找需要的头文件*/
-lstaticmath /*告诉编译器要链接libstack库*/
-static      /*对于支持动态链接的系统，使用静态链接而不是动态链接进行链接操作*/
-fPIC        /*创建与地址无关的编译程序(pic,position independent code),为了能够在多个应用程序间共享*/
-shared      /*编译成动态库*/
-lxx         /*其中xx为指定函数库，在linux下，一般库名libxx.a,libxx.so*/
```

## 4 编译过程问题总结
### 4.1 使用gcc编译C++程序，undefined reference to ...
![在这里插入图片描述](https://img-blog.csdnimg.cn/88827e9930904efa8cef13bcbab3094d.png)
没有链接C++标准库，解决方法：

```bash
gcc main.cpp -l stdc++
```
```
如：gcc -l stdc++ main.cpp 
注意：不过有时候，明明已经写上了链接库，可还是会有undefined reference的错误。这个时候，
可能就是链接顺序的问题了，当链接器从左至右抛描库文件时，第一个碰到了stdc++库，发现并
没有使用这个库中的符号，于是就将这个库给丢弃不用了，继续往后链接hello.o的时候，发现了
其中要使用一些符号，而这些符号是stdc++中的，而stdc++库已经被链接器给扔了，所以就找不
着了，就有了undefined reference。解决的方案也是两个：
(1)一是按引用顺序写链接的目标文件的顺序，如果是编译可执行程序，就从包含main函数的.o文件
开始写，最基础的库写在最右边；
(2)二是加上-Wl,--as-needed参数，这个参数会将库文件中加入NEED标识，而不管这个库文件有没
有用到（也就是告诉链接器，那个暂时没用到的库先别扔了）
```

## 参考

[参考1：概述](https://seisman.github.io/how-to-write-makefile/overview.html)

[参考2：Linux下编辑、编译、调试命令总结——gcc和gdb描述](https://www.cnblogs.com/yhjoker/p/7533438.html)
