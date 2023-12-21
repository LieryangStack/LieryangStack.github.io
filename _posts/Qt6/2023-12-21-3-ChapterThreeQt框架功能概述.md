

# 3 ALL Modules

## 3.1 Qt Essentials（基础模块）

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

## 3.2 Qt Add-Ons（附加模块）

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
