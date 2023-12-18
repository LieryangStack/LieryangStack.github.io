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

下载开源版本的在线安装软件：[https://www.qt.io/download-open-source](https://www.qt.io/download-open-source)

这里我们选择定制安装

![Alt text](/assets/Qt6/ChapterOne/Qt选择安装方式.png)

Archive复选框表示存档的版本，LTS复选框表示长期支持版本，Latest releases复选框表示最新发布的版本，Preview复选框表示技术预览版本。

![Alt text](/assets/Qt6/ChapterOne/Qt6选择安装内容.png)


**IDE**：
- Qt Creator 6.0.2。Qt Creator是开发Qt程序的IDE软件，是必须安装的，默认自动安装。
- Qt Design Studio 2.3.1-community。Qt Design Studio是QML编程设计界面的工具软件。
  
**编译工具**：

- MSVC 2019 64bit。是微软的Visual C++工具集的一部分，用于Windows平台上的应用程序开发。具体来说，MSVC（Microsoft Visual C++）是一个编译器和工具集，它使开发者能够使用C++语言开发应用程序和库。安装 `Visual Studio 2019` 就会附带 `MSVC 2019 64bit` 编译器和相关工具。
- MinGW 11.2.0 64-bit。一个用于 Windows 平台的 GCC（GNU Compiler Collection）编译器的移植版本。MinGW（Minimalist GNU for Windows）提供了一套完整的开源编程工具集，允许开发者在 Windows 上编译和运行使用 C、C++、Fortran 等语言编写的应用程序。一般使用该编译工具。
- CMake 3.21.1 64-bit。CMake是一个构建工具。Qt传统的构建工具是qmake。
- Ninja 1.10.2。Ninja是一个小型的构建系统，专注于构建速度。CMake可以和Ninja结合使用。

**模块**：
- WebAssembly（TP）。我们可以将用Qt编写的程序编译为WebAssembly格式，发布到Web浏览器上运行。
- Qt Quick 3D。这是Qt的一个模块，它为Qt Quick提供一些实现3D图形功能的APi。Qt Quick是QML的控件库。
- Qt Creator 6.0.2 Plugin Development。为Qt Creator开发插件所需的一些头文件和库文件，需要安装（补充：Qt Designer是一个窗口界面设计软件，被集成到了Qt Creator中）。
- Qt Installer Framework 4.2。这是为发布应用软件制作安装包的工具软件。
- OpenSSL 1.1.1j Tookit。安全套接字层（secure socket layer，SSL）是一种网络安全通信协议，使用SSL协议可以保障网络通信不被窃听，OpenSSL是实现了SSL协议的一个开源工具包

## 1.3 编写一个Hello World程序



### 1.3.1 Qt Creator简介

### 1.3.2 新建一个GUI项目

### 1.3.3 项目的文件组成和管理

### 1.3.4 项目的构建、调试与运行
