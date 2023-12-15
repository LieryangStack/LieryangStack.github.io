---
layout: post
title: 八、Linux中pkg-config的使用
subtitle: 
categories: 日之漫录
tags: [日之漫录]
---

meson中的dependency命令使用的是pkg-config命令，学习meson之前，先学习pkg-config。

## 1 pkg-config简单介绍
pkg-config在编译应用程序和库的时候作为一个工具来使用。例如你在命令行通过如下命令编译程序时：
```bash
gcc -o test test.c `pkg-config --libs --cflags glib-2.0`
```

pkg-config可以帮助你插入正确的编译选项，而不需要你通过硬编码的方式来找到glib(或其他库）。

`--cflags一般用于指定头文件，--libs一般用于指定库文件。`

大家应该都知道一般用第三方库的时候，就少不了要使用到第三方的头文件和库文件。我们在编译、链接的时候，必须要指定这些头文件和库文件的位置。对于一个比较大的第三方库，其头文件和库文件的数量是比较多的，如果我们一个个手动地写，那将是相当的麻烦的。因此，pkg-config就应运而生了。`pkg-config能够把这些头文件和库文件的位置指出来，给编译器使用。`pkg-config主要提供了下面几个功能：

- 检查库的版本号。 如果所需要的库的版本不满足要求，它会打印出错误信息，避免链接错误版本的库文件

- 获得编译预处理参数，如宏定义、头文件的位置

- 获得链接参数，如库及依赖的其他库的位置，文件名及其他一些链接参数

- 自动加入所依赖的其他库的设置

pkg-config命令的基本用法如下：

```bash
# pkg-config <options> <library-name>
```

```bash
[root@localhost pkgconfig]# pkg-config --list-all
zlib                      zlib - zlib compression library
gio-unix-2.0              GIO unix specific APIs - unix specific headers for glib I/O library
inputproto                InputProto - Input extension headers
cairo-xcb                 cairo-xcb - XCB surface backend for cairo graphics library
gio-2.0                   GIO - glib I/O library
//后续省略
```

## 2 配置环境变量

事实上，pkg-config只是一个工具，所以不是你安装了一个第三方库，pkg-config就能知道第三方库的头文件和库文件的位置的。为了让pkg-config可以得到一个库的信息，就要求库的提供者提供一个.pc文件。默认情况下，比如执行如下命令：

```bash
pkg-config --libs --cflags glib-2.0
```

pkg-config会到`/usr/lib/pkconfig/`目录下去寻找glib-2.0.pc文件。也就是说在此目录下的.pc文件，pkg-config是可以自动找到的。然而假如我们安装了一个库，其生成的.pc文件并不在这个默认目录中的话，pkg-config就找不到了。此时我们需要通过PKG_CONFIG_PATH环境变量来指定pkg-config还应该在哪些地方去寻找.pc文件。

我们可以通过如下命令来设置PKG_CONFIG_PATH环境变量：

```bash
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/
```

这样pkg-config就会在/usr/local/lib/pkgconfig/目录下寻找.pc文件了。我们在文章开头提到的找不到Leptonica 1.74.4的原因也正是因为其找不到lept.pc，因此我们只需要将对应的目录设置到PKG_CONFIG_PATH环境变量中即可。

另外还需要注意的是,上述环境变量的设置只对当前的终端窗口有效。为了让其永久生效，我们可以将上述命令写入到 /etc/bash.bashrc 或者 ~./bashrc 等文件中，以方便后续使用。

## 3 pkg-config与LD_LIBRARY_PATH
[详细内容参考：3、pkg-config与LD_LIBRARY_PATH](https://blog.csdn.net/fuhanghang/article/details/130206203)

## 4 pc文件书写规范
这里我们首先来看一个例子：

```bash
[root@localhost pkgconfig]# cat libevent.pc 
#libevent pkg-config source file
 
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include
 
Name: libevent
Description: libevent is an asynchronous notification event loop library
Version: 2.0.22-stable
Requires:
Conflicts:
Libs: -L${libdir} -levent
Libs.private: 
Cflags: -I${includedir}
```
这是libevent库的一个真实的例子。下面我们简单描述一下pc文件中的用到的一些关键词：

- Name：一个针对library或package的便于人阅读的名称。这个名称可以是任意的，它并不会影响到pkg-config的使用，pkg-config是采用pc文件名的方式来工作的。

- Description：对package的简短描述
URL：人们可以通过该URL地址来获取package的更多信息或者package的下载地址

- Version：指定package版本号的字符串

- Requires：本库所依赖的其他库文件。所依赖的库文件的版本号可以通过使用如下比较操作符指定：=，<，>，<=，>=

- Requires.private：本库所依赖的一些私有库文件，但是这些私有库文件并不需要暴露给应用程序。这些私有库文件的版本指定方式与Requires中描述的类似。

- Conflicts：是一个可选字段，其主要用于描述与本package所冲突的其他package。版本号的描述也与Requires中的描述类似。本字段也可以取值为同一个package的多个不同版本实例。例如：Conflicts: bar < 1.2.3, bar >= 1.3.0

- Cflags：编译器编译本package时所指定的编译选项，和其他并不支持pkg-config的library的一些编译选项值。假如所需要的library支持pkg-config，则它们应该被添加到Requires或者Requires.private中

- Libs：链接本库时所需要的一些链接选项，和其他一些并不支持pkg-config的library的链接选项值。与Cflags类似

- Libs.private：本库所需要的一些私有库的链接选项。

## 参考
[1.Linux中pkg-config的使用](https://blog.csdn.net/fuhanghang/article/details/130206203)