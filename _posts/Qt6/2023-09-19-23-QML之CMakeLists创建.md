---
layout: post
title: 二十三、QML——CMakeLists项目创建
categories: QML
tags: [QML]
---

1. 首先在文件夹 [/assets/Qt6/qml_23_CMake/](/assets/Qt6/qml_23_CMake/) 下创建一个 `Qt Design Studio` 项目。

![alt text](image.png)


2. 项目 `StudyProjectCreate` 目录内创建一个 `CMakeLists.txt` 文件。

![alt text](image-1.png)



分析：由于创建 Qt Widgets Application 和 Qt Quick Application 造成的资源文件导入问题。

CMakeLists.txt 文件中，

添加哪些就可以使得 c++ 使用资源文件

添加哪些就可以使得 qml 文件使用资源文件


https://doc.qt.io/qtdesignstudio/studio-designer-developer-workflow.html