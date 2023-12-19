---
layout: post
title: 第1章 认识Qt
categories: Qt6开发学习
tags: [Qt6开发学习]
---

Qt是一个跨平台应用开发框架（framework），它是用C++语言写的一套类库。使用Qt能为桌面计算机、服务器、移动设备甚至单片机开发各种应用（application），特别是图形用户界面（graphical user interface）程序。

## 1.1 Qt简介

### 1.1.1 Qt的跨平台开发能力

Qt具有跨平台开发能力。Qt能用于如下一些设备的平台的应用开发。

- 桌面应用开发，支持桌面操作系统包括Windows、桌面Linux、macOS。
- 手机和平板计算机等移动设备的应用开发，支持移动操作系统包括 Android、iOS和Windows。
- 嵌入式设备的应用开发，支持的嵌入式操作系统包括QNX、嵌入式Linux和VxWorks等。
- MCU的应用开发，支持嵌入式实时操作系统FreeRTOS或无操作系统。MCU的处理器只支持NXP、Renesas、ST、Infineon等公司的部分型号单片机开发板。

### 1.1.2 Qt的许可类型和安装包

根据开发目标的不同，Qt提供了3种安装包。

- Qt for Application Development：用于为计算机额移动设备开发应用的开发套件安装包，有商业和开源两种许可协议，具有Windows、Linux、macOS主机平台版本。
- Qt for Device Creation：用于嵌入式设备开发应用的开发套件安装包。
- Qt for MCUS:用于为MCU开发GUI程序的开发套件安装包，只有商业许可协议，具有Windows和Linux主机平台版本。

### 1.1.3 Qt支持的开发语言

- C++：Qt类库本身是用C++语言编写的，所以Qt支持的基本开发语言是C++。
- QML：它是类似于JavaScript1的声明性语言。Qt提供了一个用QML编写的库 `Qt Quick`，它类似于Qt C++类库，区别是Qt Quick中的各种控件被称为QML类型（type）。QML主要是创建现代感很强的界面显示。还可以混合使用QML和C++编程，也就是QML创建用户界面，用C++处理后台业务逻辑。
- Python：Qt C++类库开源被转换为Python绑定，我们可以用Python语言编程调用Qt类库进行GUI程序开发。Qt类库的Python绑定版本比较多，比较常用的是PyQt和P有Side。

### 1.1.4 Qt6新特性

Qt6.0在2020年12月正式发布，它引入了很多新的特性，主要包括如下内容：
- 支持C++17标准。
- Qt核心库的改动。
- 新的图形架构。Qt5中的3D图形API依赖OpenGL，但是现在的技术环境发生了很大的变化。在Linux平台上，Vulkan逐渐取代OpenGL，苹果公司主推其Metal，Microsoft公司则是使用Direct 3D。为了使用不同平台上的3D技术，Q6设计了3D图形的渲染硬件接口（rendering hardware interface，RHI）。RHI是3D图形系统的一个抽象层，使得Qt可以使用平台本地化的3D图形API。
- CMake构建系统。Qt6支持CMake构建系统，并且建议新的项目使用CMake，Qt6本身就是用Cmake构建的。

## 1.2 Qt的安装

这一小节没有根据书本内容编排，根据我对Qt的使用进行补充编写。

### 1.2.1 下载Qt安装软件

下载开源版本的在线安装软件：[https://www.qt.io/download-open-source](https://www.qt.io/download-open-source)，这里我们选择定制安装。

安装可能比较慢，可以使用镜像 `.\qt-unified-windows-x86-online.exe --mirror https://mirrors.ustc.edu.cn/qtproject`

![Alt text](/assets/Qt6/ChapterOne/Qt选择安装方式.png)

- **Archive**：复选框表示存档的版本
- **Latest supported releases**：复选框表示最新发布的版本
- **Preview**：复选框表示技术预览版本

![Alt text](/assets/Qt6/ChapterOne/Qt6选择安装内容.png)

我选择安装以上组件，各组件有什么作用，我在下一节进行总结，我的Qt在Windows下被成功安装在 `C:\Qt`。

![Alt text](/assets/Qt6/ChapterOne/Qt安装目录组成.png)

- `5.15.2` 文件夹里面的 `mingw` 目录包含的是 Qt 的类库文件，例如头文件、静态库、动态库等，这些类库文件使用 MinGW 工具集编译而成。

- `Tools` 文件夹里面的 `mingw` 目录包含的是 MinGW 工具集，例如编译器 g++、链接器 ld、make 工具、打包工具 ar 等。

- `vcredist` 文件夹存储 VC 运行库安装文件。


### 1.2.2 Qt有哪些组件

首先应该先介绍一下Qt有哪些界面设计软件和IDE。

#### 1.2.2.1 Qt界面设计软件和IDE

**Qt Designer**

Qt设计师（Qt Designer）是Qt界面设计软件，也就是 `*.ui` 文件编辑软件。该软件跟随Qt版本和编译工具链都会编译生成该界面设计软件，只要安装编译工具链就会附带该软件安装。

![Alt text](/assets/Qt6/ChapterOne/QtDesigner.png)

由于编译工具链和Qt版本的不同，会有不同的`Qt Designer`，该软件会被集成在`Qt Creator`中使用线程调用。

![Alt text](/assets/Qt6/ChapterOne/QtDesigner文件路径.png)

**Qt Creator**

`Qt Creator`是集成编译环境IDE，可以进行界面设计，编辑编译程序。点击Design就可以调用`Qt Designer`软件。

![Alt text](/assets/Qt6/ChapterOne/QtCreator.png)

`Qt Creator`在 Developer and Designer Tools 里面可以选择安装。`Qt Creator`使用微软的MSVC编译的，安装目录是在 `C:\Qt\Tools\QtCreator`，同时由于使用MSVC编译，Qt安装目录还会下载VC相关组件安装包 `C:\Qt\vcredist`。

![Alt text](/assets/Qt6/ChapterOne/QtCreator如何安装.png)

![Alt text](/assets/Qt6/ChapterOne/vcredist.png)

**Qt Design Studio**

Qt Design Studio 4.1.0 LTS。Qt Design Studio是QML编程设计界面的工具软件, `LTS` 表示长期支持版本。该软件也是使用MSVC编译生成的，安装目录是 `C:\Qt\Tools\QtDesignStudio-4.1.0-lts`。

![Alt text](/assets/Qt6/ChapterOne/QtDesignStudio.png)

#### 1.2.2.2 Qt编译工具链和构建系统组件

1. `MSVC` 和 `MinGw` 是编译工具链。
2. `Ninja` 和 `Make`都是构建系统。
3. `CMake` 和 `qmake` 是元构建系统，生成构建系统`Make`需要的`Makefile`文件，`Make` 或者 `Ninja` 调用编译工具链生成可执行文件。


**编译工具链**

- **MSVC 2019 64bit**：是微软的Visual C++工具集的一部分，用于Windows平台上的应用程序开发。具体来说，MSVC（Microsoft Visual C++）是一个编译器和工具集，它使开发者能够使用C++语言开发应用程序和库。安装 `Visual Studio 2019` 就会附带 `MSVC 2019 64bit` 编译器和相关工具。

- **MinGW 11.2.0 64-bit**：一个用于 Windows 平台的 GCC（GNU Compiler Collection）编译器的移植版本。MinGW（Minimalist GNU for Windows）提供了一套完整的开源编程工具集，允许开发者在 Windows 上编译和运行使用 C、C++、Fortran 等语言编写的应用程序。一般使用该编译工具。

**构建系统**

- **CMake 3.21.1 64-bit**：CMake是一个构建工具（元构建系统）。Qt传统的构建工具是qmake。

- **Ninja 1.10.2**：Ninja是一个小型的构建系统，专注于构建速度。CMake可以和Ninja结合使用。

#### 1.2.2.3 WebAssembly配置

WebAssembly（缩写为wasm）是一种使用非JavaScript代码，并使其在浏览器中运行的方法。这些代码可以是C、C++或Rust等。它们会被编译进你的浏览器，在你的 CPU 上以接近原生的速度运行。WebAssembly就是用来解决JavaScript 当前存在的一些效率等问题，WebAssembly不能替代 Javascript，相反，这两种技术是相辅相成的。通过 JavaScript API，你可以将 WebAssembly模块加载到你的页面中。也就是说，你可以通过 WebAssembly来充分利用编译代码的性能，同时保持 JavaScript 的灵活性。

WebAssembly作为一种小型且快速的**二进制格式编程语言**，它可以为Web应用提供接近原生的性能。

**安装Emscripten**

Emscripten是一个编译到WebAssembly的工具链。它可以让你在没有浏览器插件的情况下以接近本地的速度在网络上运行Qt。

1. <spand style="color:red;">一定要先安装Python</pasn>
2. 下载emsdk：[https://github.com/emscripten-core/emsdk](https://github.com/emscripten-core/emsdk)
3. 

https://doc.qt.io/qt-6/wasm.html
https://emscripten.org/docs/getting_started/downloads.html
https://blog.csdn.net/c1s2d3n4cs/article/details/122739295

![Alt text](image.png)

#### 1.2.2.4 Qt其他组件

- Qt Quick 3D。这是Qt的一个模块，它为Qt Quick提供一些实现3D图形功能的APi。Qt Quick是QML的控件库。
- Qt Creator 6.0.2 Plugin Development。为Qt Creator开发插件所需的一些头文件和库文件。
- Qt Installer Framework 4.2。这是为发布应用软件制作安装包的工具软件。
- OpenSSL 1.1.1j Tookit。安全套接字层（secure socket layer，SSL）是一种网络安全通信协议，使用SSL协议可以保障网络通信不被窃听，OpenSSL是实现了SSL协议的一个开源工具包


## 1.3 编写一个Hello World程序



### 1.3.1 Qt Creator简介

### 1.3.2 新建一个GUI项目

### 1.3.3 项目的文件组成和管理

### 1.3.4 项目的构建、调试与运行
