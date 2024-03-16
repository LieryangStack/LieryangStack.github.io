---
layout: post
title: GStreamer核心对象-前言
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GStreamer核心对象如何学习

每一章都是如何布置结构，如何去学习

1. 核心对象基本概念，基本概念基本都来自GStreamer官网开头的前几段。

2. 核心对象基本结构。

    - 核心对象类型注册定义，例如：G_DEFINE_TYPE、G_DEFINE_BOXED_TYPE等等。

    - 这一节要包含结构体定义（头文件里面的结构体和源文件里面的结构体），特别要注意非标准GObject对象，可能会通过头文件是对象Public成员，源文件是对象Private成员。标准GObject对象可能会有私有结构体。

    - 源文件和头文件中的枚举类型，每种枚举有什么作用。

3. 根据功能给函数分类，难以理解的函数要附带示例程序。

## 2 GStreamer核心对象示例程序目录

核心对象示例程序目录在 [/assets/GStreamerStudy/CoreObject/](/assets/GStreamerStudy/CoreObject/)，子目录会根据章节序号排序。示例程序来源一般是GStreamer项目里面的测试程序改编。可能还包含对核心对象定义源文件（对其中的函数进行添加注释）。

## 3 学习感悟

### 通过学习 GstStructure 感悟

虽然不是标准的`GObject`对象，GBoxed对象，创建实际结构体空间的时候，可以根据层级继承结构体，只创建最终继承结构体的内存。

### 对象

- 能够实例化 （能够实例化也就是能够类化）

- 能够类化

- 接口类型

- Boxed类型

1. 是GObject类型对象

  - 是否是抽象对象(不可以实例化对象)，只能被继承

2. 接口对象

3. GBoxed对象

示例程序可以从源文件测试程序中寻找 `gstreamer-1.22.6/subprojects/gstreamer/tests/check/gst/gststructure.c`

### 内存

- 申请内存相关函数

- 释放内存相关函数

`GObject` 标准对象可以自动调用释放内存函数，非 `GObject` 标准对象不可以自动调用释放内存函数，需要手动调用，或者自己实现引用计数。

