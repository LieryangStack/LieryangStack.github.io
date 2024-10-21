---
layout: post
title: 二十三、QML——CMakeLists项目创建
categories: QML
tags: [QML]
---

本章节记录，如何创建 `Qt Designer Studio` 和 `Qt Creator` 共同开发项目。

## 1 创建共同开发项目

### 1.1 创建一个 `Qt Designer Studio` 项目

创建一个 `Qt Designer Studio` 项目

![alt text](image.png)

使能该项目 `Enable CMake Generator`

![alt text](image-1.png)

### 1.2 使用 `Qt Creator` 打开CMakeLists

![alt text](image-2.png)

### 1.3 `Qt Creator` 编译失败问题

#### 1.3.1 由于 `Windows` 长路径问题，导致编译失败。

![alt text](image-3.png)

**解决方案**：

通常是使用较短的路径，比如：`C:\dev`

`Qt Creator 13` 提出了一种不同的解决方法，对源项目目录和构建目录使用**软连接**。

一共有两种方法，一个是对于所有使用 `Qt Creator` 项目都使用，另一个是仅仅对目前项目使用。

第一种：通过设置 `CMake` 对所有项目都启用。

![alt text](image-4.png)

第二种：通过设置 `CMakeLists.txt.shared` 单独启用 `QTC_CMAKE_USE_JUNCTIONS` 

![alt text](image-6.png)

通过在软连接目录创建和删除问题，相应的源目录的文件也会变化。

![alt text](image-5.png)

## 2 项目CMakeLists分析



## 参考

[参考1：Designer-Developer Workflow](https://doc.qt.io/qtdesignstudio/studio-designer-developer-workflow.html)

[参考2：长路径问题](https://www.qt.io/blog/qt-creator-13-cmake-update)