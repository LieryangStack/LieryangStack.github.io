---
layout: post
title: 三、Qt6CMake——qds生成CMakeLists
categories: Qt6CMake
tags: [Qt6CMake]
---



`qds` 就是`Qt Design Studio`，本章节记录，如何创建 `Qt Design Studio` 和 `Qt Creator` 共同开发项目。

1. 第一节实现了 `Qt Design Studio` 生成 CMakeList 文件，然后使用 `Qt Creator` 编辑。

2. 第二节是对整个项目文件的分析

3. 第三节想把C++对象封装成QML插件，然后使用 `Qt Design Studio` 访问，实例化创建对象。

## 1 创建共同开发项目

### 1.1 创建一个 `Qt Design Studio` 项目

创建一个 `Qt Design Studio` 项目

![alt text](/assets/Qt6/cmake_03_qds/image/image.png)

使能该项目 `Enable CMake Generator`

![alt text](/assets/Qt6/cmake_03_qds/image/image-1.png)

### 1.2 使用 `Qt Creator` 打开CMakeLists

![alt text](/assets/Qt6/cmake_03_qds/image/image-2.png)

### 1.3 `Qt Creator` 编译失败问题

#### 1.3.1 由于 `Windows` 长路径问题，导致编译失败。

![alt text](/assets/Qt6/cmake_03_qds/image/image-3.png)

**解决方案**：

通常是使用较短的路径，比如：`C:\dev`

`Qt Creator 13` 提出了一种不同的解决方法，对源项目目录和构建目录使用**软连接**。

一共有两种方法，一个是对于所有使用 `Qt Creator` 项目都使用，另一个是仅仅对目前项目使用。

第一种：通过设置 `CMake` 对所有项目都启用。

![alt text](/assets/Qt6/cmake_03_qds/image/image-4.png)

第二种：通过设置 `CMakeLists.txt.shared` 单独启用 `QTC_CMAKE_USE_JUNCTIONS` 

![alt text](/assets/Qt6/cmake_03_qds/image/image-6.png)

通过在软连接目录创建和删除问题，相应的源目录的文件也会变化。

![alt text](/assets/Qt6/cmake_03_qds/image/image-5.png)

## 2 项目CMakeLists分析
主目录下的 `CMakeLists.txt` 会调用以下几个部分构建应用程序 `qds.cmake`、`App`、`cmake`、`StudyProject`、`StudyProjectContent` 里面的 `CMakeLists.txt` 或 `***.cmake` 文件。

![alt text](/assets/Qt6/cmake_03_qds/image/image-7.png)

![alt text](/assets/Qt6/cmake_03_qds/image/image-8.png)

![alt text](/assets/Qt6/cmake_03_qds/image/image-9.png)

### 2.1 关于qml插件库引用


![alt text](/assets/Qt6/cmake_03_qds/image/image-13.png)


比如 `StudyProject` 目录里面定义了

![alt text](/assets/Qt6/cmake_03_qds/image/image-10.png)

`qds.cmake` 却只调用了 `libStudyProjectContentplugin.a` 静态库，但是看编译文件 `build.ninja` ，编译应用程序的时候也调用了 `libStudyProjectContent.a` 静态库。

<font color="blue">因为 `libStudyProjectContentplugin.a` 编译的时候是依赖  `libStudyProjectContent.a` 静态库。</font>

![alt text](/assets/Qt6/cmake_03_qds/image/image-11.png)

### 2.2  qmlcomponents插件库

这部分是 `Qt Design Studio` 定义的 qml组件，比如：`ArcItem`、`Glow` 、`QuickStudioApplication` 等等。

![alt text](/assets/Qt6/cmake_03_qds/image/image-12.png)

## 3 创建C++插件导入qds

制作插件（或者说是动态库）是很常见的需求，其实也可以说是将qml文件或者c++文件封装成静态库或者动态库，提供moudle信息，然后给其他qml界面调用。

1. C++编写的对象组件和qml编写的对象组件都可以一起封装成一个插件。

2. CMake中使用 `qt_add_qml_module` 直接生成相关的文件和库。（`qt_add_qml_module` 等同于 `qt6_add_qml_module`）

### 3.1 生成Qt Quick扩展插件

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image.png)

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-1.png)

### 3.2 分析生成的扩展插件组成

生成前只有三个文件，使用 `qt_add_qml_module` 生成相关静态库和类型描述文件：

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-2.png)

`qt_add_qml_module` 生成相关的文件分析：

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-4.png)

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-3.png)

注意：`qmldir`文件和 `Test.qmltypes` 文件

- **qmldir**： 是用来描述插件的相关信息。文件中的 `typeinfo` 指明当前插件对应的 .qmltypes 文件是什么。

- **Test.qmltypes**：用于定义 QML 类型的元信息<font color="blue">（好像只是用来描述C++文件定义的对象信息）</font>

- 如果使用该 `Test` 插件（模块），只需要调用 `libTestplugin.a` 库即可。也就是 `target_link_libraries(ExampleProject PRIVATE Qt6::Quick Testplugin)` 就可以。（因为 `libTestplugin.a` 库会链接 `libTest.a` 库）

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-5.png)

### 3.3 插件无法再qds中使用

![alt text](/assets/Qt6/cmake_03_qds/extension_plugin_image/image-6.png)

原因：

动态库 Liplugin.dll 链接 Li.dll 造成的问题。

解决方案：

1. 使用和Qt Design Studio相同版本的Qt和编译工具。

2. 一定要添加 `PLUGIN_TARGET 插件库名称` 与 `qt6_add_qml_module` 目标名称相同。

    ```c
    qt_add_qml_module(chartsplugin
        URI "Charts"
        PLUGIN_TARGET chartsplugin
        DEPENDENCIES QtQuick
    )
    ```

## 参考

[参考1：Designer-Developer Workflow](https://doc.qt.io/qtdesignstudio/studio-designer-developer-workflow.html)

[参考2：长路径问题](https://www.qt.io/blog/qt-creator-13-cmake-update)
