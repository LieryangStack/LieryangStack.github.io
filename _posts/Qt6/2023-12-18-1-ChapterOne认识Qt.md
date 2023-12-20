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

>此时我的Qt修改了安装版本，此时为Qt6.6.1。

Emscripten是一个编译到WebAssembly的工具链。它可以让你在没有浏览器插件的情况下以接近本地的速度在网络上运行Qt。

1. <spand style="color:red;">一定要先安装Python</spand>，下载链接：[https://www.python.org/downloads/windows/](https://www.python.org/downloads/windows/)
2. 下载emsdk：[https://github.com/emscripten-core/emsdk](https://github.com/emscripten-core/emsdk)
    ```sh
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    git pull
    ```
3. 不同版本的Qt都会有对应的emsdk版本：[https://doc.qt.io/qt-6/wasm.html](https://doc.qt.io/qt-6/wasm.html)
    ```
    Qt 6.2: 2.0.14
    Qt 6.3: 3.0.0
    Qt 6.4: 3.1.14
    Qt 6.5: 3.1.25
    Qt 6.6: 3.1.37
    ```

4. Windows和Linux下安装方法参考：[https://emscripten.org/docs/getting_started/downloads.html](https://emscripten.org/docs/getting_started/downloads.html)
    ```sh
    # windows下安装方法
    ./emsdk.bat install 3.1.37
    ./emsdk.bat activate 3.1.37
    ./emsdk_env.bat
    
    # Linux下安装方法
    ./emsdk install 3.1.37
    ./emsdk activate 3.1.37
    source ./emsdk_env.sh
    ```
5. 设定环境变量（脚本设定的只是终端临时环境变量）
![Alt text](/assets/Qt6/ChapterOne/emsdk环境变量设定.png)

1. 测试是否安装成功
![Alt text](/assets/Qt6/ChapterOne/emsdk是否安装成功.png)

1. Qt设定emsdk
![Alt text](/assets/Qt6/ChapterOne/image.png)
![Alt text](/assets/Qt6/ChapterOne/image-1.png)
![Alt text](/assets/Qt6/ChapterOne/image-2.png)
![Alt text](/assets/Qt6/ChapterOne/image-3.png)


#### 1.2.2.4 Qt其他组件

- Qt Quick 3D。这是Qt的一个模块，它为Qt Quick提供一些实现3D图形功能的APi。Qt Quick是QML的控件库。
- Qt Creator 6.0.2 Plugin Development。为Qt Creator开发插件所需的一些头文件和库文件。
- Qt Installer Framework 4.2。这是为发布应用软件制作安装包的工具软件。
- OpenSSL 1.1.1j Tookit。安全套接字层（secure socket layer，SSL）是一种网络安全通信协议，使用SSL协议可以保障网络通信不被窃听，OpenSSL是实现了SSL协议的一个开源工具包


## 1.3 编写一个Hello World程序

### 1.3.1 Qt Creator编译环境

![Alt text](/assets/Qt6/ChapterOne/编译工具链-1.png)
![Alt text](/assets/Qt6/ChapterOne/编译工具链-2.png)
![Alt text](/assets/Qt6/ChapterOne/编译工具链-3.png)
![Alt text](/assets/Qt6/ChapterOne/编译工具链-4.png)
![Alt text](/assets/Qt6/ChapterOne/编译工具链-5.png)

### 1.3.2 Qt 交叉编译环境

https://download.qt.io/official_releases

源文件路径：C:\Qt\SourcesCode\qt-everywhere-src-6.6.1
GCC编译器路径：C:\Program Files (x86)\Arm GNU Toolchain aarch64-none-linux-gnu\13.2 Rel1

![Alt text](image.png)

### 1.3.2 新建一个GUI项目

### 1.3.3 项目的文件组成和管理

### 1.3.4 项目的构建、调试与运行

# 2 ALL Modules

## 2.1 Qt Essentials（基础模块）

| 模块名称           | 描述                                                         |
| ------------------ | ------------------------------------------------------------ |
| Qt Core        | 其他模块使用的核心非图形类。                                   |
| Qt D-Bus       | 用于通过D-Bus协议进行进程间通信的类。                          |
| Qt GUI         | 图形用户界面（GUI）组件的基类。                                |
| Qt Network     | 使网络编程更简单、更便携的类。                                 |
| Qt QML         | 用于QML和JavaScript语言的类。                                  |
| Qt Quick       | 用于构建具有自定义用户界面的高度动态应用程序的声明式<spand style="color:red;">框架</spand>。         |
| Qt Quick Controls | 提供轻量级的QML类型，用于为桌面、嵌入式和移动设备创建高性能用户界面。这些类型采用简单的样式架构，非常高效。 |
| Qt Quick Dialogs | 用于从Qt Quick应用程序创建和交互系统对话框的类型。              |
| Qt Quick Layouts | 布局是用于在用户界面中排列基于Qt Quick 2的项目的项目。          |
| Qt Quick Test   | QML应用程序的单元测试框架，测试用例以JavaScript函数的形式编写。**注意：二进制兼容性保证不适用于Qt Quick Test。但它将保持源代码兼容。** |
| Qt Test        | 用于对Qt应用程序和库进行单元测试的类。**注意：二进制兼容性保证不适用于Qt Test。但它将保持源代码兼容。** |
| Qt Widgets     | 用于通过C++小部件扩展Qt GUI的类。                               |

## 2.2 Qt Add-Ons（附加模块）

Qt附加模块为特定目的提供了额外的价值。这些模块可能只在某些开发平台上可用。许多附加模块要么是功能完整且为了向后兼容而存在，要么只适用于特定平台。每个附加模块分别指定其兼容性承诺。

Qt安装程序包括下载附加组件的选项。欲了解更多信息，请访问Qt入门页面。

下表列出了Qt附加模块：

| 模块名称 | 开发平台 | 目标平台 | 描述 |
| ------- | ------- | ------- | ---- |
| Active Qt | Windows | Windows | 用于使用ActiveX和COM的应用程序的类。 |
| Qt 3D | 所有 | 所有 | 支持2D和3D渲染的近实时仿真系统的功能。 |
| Qt 5 Core Compatibility APIs | 所有 | 所有 | Qt 5中但不在Qt 6中的Qt Core APIs。 |
| Qt Bluetooth | 所有 | Android, iOS, Linux, Boot to Qt, macOS, Windows | 提供对蓝牙硬件的访问。 |
| Qt Concurrent | 所有 | 所有 | 编写多线程程序的类，无需使用低级线程原语。 |
| Qt Help | 所有 | 所有 | 将文档集成到应用程序中的类。 |
| Qt Image Formats | 所有 | 所有 | 提供额外图像格式的插件：TIFF, MNG, TGA, WBMP。 |
| Qt Multimedia | 所有 | 所有 | 提供丰富的QML类型和C++类来处理多媒体内容。还包括处理相机访问的API。 |
| Qt NFC | 所有 | Android, iOS, macOS, Linux, Windows | 提供对近场通信（NFC）硬件的访问。在桌面平台上，NDEF访问仅支持Type 4标签。 |
| Qt OPC UA | 所有 | 所有（除QNX, WebAssembly外） | 用于工业应用中数据建模和数据交换的协议。 |
| Qt OpenGL | 所有 | 所有 | 使在Qt应用程序中使用OpenGL变得容易的C++类。一个单独的Qt OpenGL Widgets C++类库提供了一个用于渲染OpenGL图形的小部件。 |
| Qt PDF | Windows, Linux, macOS | - | 提供渲染PDF文档的类和函数。 |
| Qt Positioning | 所有 | Android, iOS, macOS, Linux, Windows | 提供位置、卫星信息和区域监控类。 |
| Qt Print Support | 所有 | 所有（iOS除外） | 使打印更容易和更便携的类。 |
| Qt Quick Widgets | 所有 | 所有 | 提供用于显示Qt Quick用户界面的C++小部件类。 |
| Qt Quick Effects | 所有 | 所有 | 提供QML类型以对Qt Quick项目应用一个或多个简单的图形效果。 |
| Qt Quick Particles | 所有 | 所有 | 提供粒子效果的QML类型。 |
| Qt Remote Objects | 所有 | 所有 | 提供在进程或设备之间共享QObject的API（属性/信号/槽）的简单机制。 |
| Qt SCXML | 所有 | 所有 | 提供从SCXML文件创建状态机并将其嵌入到应用程序中的类和工具。 |
| Qt Sensors | 所有 | Android, iOS, Windows | 提供对传感器硬件的访问。 |
| Qt Serial Bus | 所有 | Linux, Boot to Qt, macOS, Windows | 提供对串行工业总线接口的访问。目前，该模块支持CAN总线和Modbus协议。 |
| Qt Serial Port | 所有 | Linux, Boot to Qt, macOS, Windows | 提供与硬件和虚拟串行端口交互的类。 |
| Qt Shader Tools | 所有 | 所有 | 提供跨平台Qt着色器管线的工具。这些使得处理图形和计算着色器以使其适用于Qt Quick和Qt生态系统中的其他组件成为可能。 |
| Qt Spatial Audio | 所有 | 所有 | 提供对空间音频的支持。在3D空间中创建声音场景，包含不同的声源和与房间相关的属性，如混响。 |
| Qt SQL | 所有 | 所有 | 使用SQL的数据库集成类。 |
| Qt State Machine | 所有 | 所有 | 提供创建和执行状态图的类。 |
| Qt SVG | 所有 | 所有 | 用于显示SVG文件内容的类。支持SVG 1.2 Tiny标准的一个子集。一个单独的Qt SVG Widgets C++类库提供了在小部件UI中渲染SVG文件的支持。 |
| Qt TextToSpeech | 所有 | 所有 | 提供将文本合成为语音并作为音频输出播放的支持。 |
| Qt UI Tools | 所有 | 所有 | 提供在运行时动态加载Qt Designer中创建的基于QWidget的表单的类。 |
| Qt WebChannel | 所有 | 所有 | 提供从HTML客户端访问QObject或QML对象，以实现Qt应用程序与HTML/JavaScript客户端的无缝集成。 |
| Qt WebEngine | Windows, Linux, macOS | - | 提供在应用程序中嵌入Web内容的类和函数，使用Chromium浏览器项目。 |
| Qt WebSockets | 所有 | 所有 | 提供符合RFC 6455的WebSocket通信。 |
| Qt WebView | 所有支持的平台 | - | 使用平台原生API在QML应用程序中显示Web内容，无需包含完整的Web浏览器堆栈。 |
| Qt XML | 所有 | 所有 | 使用文档对象模型（DOM）API处理XML。 |
| Qt Charts          | 所有     | 所有                           | 用于展示视觉上令人愉悦的图表的UI组件，由静态或动态数据模型驱动。 |
| Qt CoAP            | 所有     | 所有                           | 实现由RFC 7252定义的CoAP的客户端。                           |
| Qt Data Visualization | 所有   | 所有                           | 创建惊艳的3D数据可视化的UI组件。                             |
| Qt Lottie Animation | 所有    | 所有                           | 一个用于渲染JSON格式的图形和动画的QML API，由Adobe® After Effects的Bodymovin插件导出。 |
| Qt MQTT            | 所有     | 所有                           | 提供MQTT协议规范的实现。                                     |
| Qt Network Authorization | 所有 | 所有                           | 提供对基于OAuth的在线服务授权的支持。                        |
| Qt Quick 3D        | 所有     | 所有                           | 提供一个高级API，用于基于Qt Quick创建3D内容或UI。             |
| Qt Quick 3D Physics | 所有    | 除QNX和INTEGRITY外的所有支持平台 | 为Qt Quick 3D提供物理模拟功能的高级QML模块。                  |
| Qt Quick Timeline  | 所有     | 所有                           | 启用基于关键帧的动画和参数化。                               |
| Qt Virtual Keyboard | 所有    | Linux和Windows桌面，以及Boot to Qt目标 | 一个用于实现不同输入方法以及一个QML虚拟键盘的框架。支持本地化键盘布局和自定义视觉主题。 |
| Qt Wayland Compositor | Linux | Linux和Boot to Qt目标           | 提供开发Wayland合成器的框架。                                 |

以下是仍在开发中但作为技术预览版可用的Qt附加模块：

| 模块名称       | 开发平台 | 目标平台 | 描述                                                         |
| ------------- | ------- | ------- | ------------------------------------------------------------ |
| Qt HTTP Server | 所有     | 所有     | 一个用于将HTTP服务器嵌入到Qt应用程序中的框架。                 |
| Qt Protobuf    | 所有     | 所有     | 提供从protobuf规范生成基于Qt的类的能力。                      |
| Qt Graphs      | 所有     | 所有     | 提供在3D中作为条形图、散点图和曲面图可视化数据的功能。         |
| Qt GRPC        | 所有     | 所有     | 提供从用于与gRPC®服务通信的protobuf规范生成基于Qt的类的能力。  |
| Qt Location    | 所有     | 所有     | 提供QML和C++接口来创建具有位置感知的应用程序。                 |
