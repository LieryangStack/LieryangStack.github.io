---
layout: post
title: 二、QtCore——The Meta-Object System
categories: QtCore
tags: [QtCore]
---

Qt的元对象系统为对象间通信提供了：

1. 信号和槽机制

2. 运行时类型信息

3. 动态属性系统

元对象系统基于三个要素：

1. QObject作为基类，可以为被继承对象<font color="red">提供元对象系统。</font>

2. 类声明的私有部分中的Q_OBJECT宏用于<font color="red">启用元对象功能</font>，如动态属性、信号和槽。

3. 元对象编译器（moc）为每个QObject子类提供实现元对象功能所需的代码。

moc工具读取C++源文件。如果它发现一个或多个包含Q_OBJECT宏的类声明，它将生成另一个C++源文件，其中包含这些类的元对象代码。这个生成的源文件要么被#include到类的源文件中，要么更常见的是，与类的实现一起编译和链接。

除了提供对象间通信的信号和槽机制（引入系统的主要原因）外，元对象代码还提供以下附加功能：

- QObject::metaObject() 返回类的相关元对象。

- QMetaObject::className() 在运行时返回类名字符串，无需通过C++编译器支持本地运行时类型信息（RTTI）。

- QObject::inherits() 函数返回一个对象是否是在QObject继承树中继承指定类的类的实例。

- QObject::tr() 用于国际化翻译字符串。

- QObject::setProperty() 和 QObject::property() 动态地按名称设置和获取属性。

- QMetaObject::newInstance() 构造类的新实例。


https://blog.csdn.net/qq_21980099/article/details/119055774
https://blog.csdn.net/weixin_41199566/article/details/135102491
https://blog.csdn.net/MrHHHHHH/article/details/134841714
https://blog.csdn.net/m0_73443478/article/details/129026374
https://blog.csdn.net/m0_73482095/article/details/135385841
https://blog.csdn.net/lucust/article/details/132079640
https://blog.csdn.net/m0_60259116/article/details/129639058

https://blog.csdn.net/qq_16488989/article/details/109204955
https://blog.csdn.net/hitzsf/article/details/108007919