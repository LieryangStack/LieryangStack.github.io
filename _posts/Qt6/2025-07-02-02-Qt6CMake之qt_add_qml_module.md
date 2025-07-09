---
layout: post
title: 二、Qt6CMake——qt_add_qml_module
categories: Qt6CMake
tags: [Qt6CMake]
---

## 1 前言

1. `qt_add_qml_module` 用于创建和管理QML模块。可以创建QML模块组件。

2. `qt_add_qml_module` 和 `qt6_add_qml_module` 可以认为是相同的，只是qt6表示了版本，qt_没有版本指定，其实本质调用也是 `qt6_add_qml_module`。

在调用qt_add_qml_module时，它会执行以下操作：

1. 创建一个动态链接库。作为QML模块的插件——该库将包含定义自定义QML元素的C++类和其他资源。

    或者创建一个静态链接库。

2. 自动创建一个qrc文件，该文件将包含指定源文件中的所有资源，例如图片、样式表和翻译文件。

3. 生成一个描述该QML模块的 `qmldir` 文件，其中包含URI、版本号、导出对象名等信息。

4. 将QML插件动态链接库和描述文件复制到项目二进制目录的特定子目录中。该位置取决于是单个项目还是多个项目，以及是在构建目录还是安装目录中构建应用程序/库。

## 2 参数

qt_add_qml_module 函数提供了多个参数，以便您在创建 QML 模块时进行配置。以下是按功能分类的参数列表：

1. 模块基本信息：

   - TARGET：目标名称，用于指定模块关联的可执行文件或库。

   - URI：模块的 URI，用于在 QML 代码中 import。

   - VERSION：模块的版本号。

2. QML 文件和资源：

   - QML_FILES：QML 文件列表，包括所有要编译到模块中的 QML 文件。

   - SOURCES：源文件列表，包括所有要编译到模块中的源代码文件和资源文件。可以包含 QML 文件。

3. 其他资源文件：

   - RESOURCE_FILES：要包含在模块中的其他资源文件，例如图片、音频等。

   - STATIC_RESOURCE_FILES：静态资源文件，类似于 RESOURCE_FILES，但会嵌入到二进制文件中。

   - RESOURCE_PREFIX：资源文件的前缀路径。

4. 输出设置：

   - OUTPUT_DIRECTORY：模块输出的目录。

   - DEBUG_OUTPUT_DIRECTORY：模块在调试模式下的输出目录。

   - RELEASE_OUTPUT_DIRECTORY：模块在发布模式下的输出目录。

5. 其他选项：

   - INSTALL_QMLDIR：用于指定安装模块时的 QML 目录。

   - NO_PLUGIN_OPTION：禁用生成插件文件的选项。

这些参数可以根据您的需求进行选择和配置，以便为您的项目创建适当的 QML 模块。

## 3 重要参数说明

![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image.png)

1. `TARGET` 和 `PLUGIN_TARGET` 最好名称一样，如果不指定 `PLUGIN_TARGET`，会默认生成 `TARGET+plugin` 的库文件。这样会有两个动态库文件，使用qds的时候会报错，不能正常链接动态库。

    ![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-2.png)

    ![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-1.png)

2. `URI` 和 `VERSION` 
   
    ![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-3.png)

## 4 问题总结

### 4.1 动态库

`TARGET` 和 `PLUGIN_TARGET` 名称不一样，就会有两个库。这样如果使用Module组件，就会报错无法加载库 C 。这是因为只调用了 `exampleplugin.dll`，调用不到 `example.dll`。所以一般要显式定义 `PLUGIN_TARGET`。

![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-4.png)

### 4.2 静态库


1. `qt_add_library(example STATIC)` 指定为静态库。

2. 使用该插件的C++源文件需要添加：`Q_IMPORT_QML_PLUGIN(examplePlugin)`。

**Q_IMPORT_QML_PLUGIN：**

`Q_IMPORT_QML_PLUGIN` 是 Qt 提供的一个宏，其主要作用是 在静态编译（静态链接）Qt 插件时，确保插件被正确地注册并可以在 QML 中使用。

在 Qt 中，QML 插件通常以动态库的形式存在（.so、.dll 等），可以在运行时自动发现和加载。

但当你使用 静态编译（也就是将 Qt 库和插件编译进可执行文件中）时，Qt 无法自动发现这些 QML 插件，因此需要手动告诉 Qt “我要导入这个插件”。

![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-5.png)

`Q_IMPORT_QML_PLUGIN` 后面的名称就是 `qmldir` 文件中的 `classname`名称。

这个类名定义规范：当你在 CMake 中使用 qt_add_qml_module() 添加一个 QML 模块时，Qt 会自动为你生成一个插件扩展类。默认情况下，这个类的名字来源于 URI 参数，将其中的 . 替换为 _ 并且最后加Plugin。

![alt text](/assets/Qt6/cmake_02_qt_add_qml_module/image/image-6.png)


## 参考

[参考1： Qt cmake 增加qml文件：深度剖析Qt cmake 的qt_add_qml_module函数](https://blog.csdn.net/qq_21438461/article/details/130475251)