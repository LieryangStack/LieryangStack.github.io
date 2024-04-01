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

可以使用 qobject_cast() 在 QObject 类上执行动态转换。qobject_cast() 函数的行为类似于标准 C++ 的 dynamic_cast()，但有两个优点：不需要 RTTI 支持，且可以跨动态库边界工作。它尝试将其参数转换为尖括号中指定的指针类型，如果对象是正确的类型（在运行时确定），则返回非零指针；如果对象类型不兼容，则返回 nullptr。

例如，假设 MyWidget 从 QWidget 继承，并且用 Q_OBJECT 宏声明：

```c++
QObject *obj = new MyWidget;
```

变量 obj 的类型是 QObject *，实际上指向一个 MyWidget 对象，因此我们可以适当地转换它：

```c++
QWidget *widget = qobject_cast<QWidget *>(obj);
```

从 QObject 到 QWidget 的转换是成功的，因为对象实际上是一个 MyWidget，它是 QWidget 的子类。既然我们知道 obj 是一个 MyWidget，我们也可以将它转换为 MyWidget *：

```c++
MyWidget *myWidget = qobject_cast<MyWidget *>(obj);
```

转换到 MyWidget 是成功的，因为 qobject_cast() 不区分内置的 Qt 类型和自定义类型。

```c++
QLabel *label = qobject_cast<QLabel *>(obj);
// label is 0
```

另一方面，转换为 QLabel 失败。然后指针被设置为 0。这使得我们可以在运行时根据类型不同地处理不同类型的对象：

```c++
if (QLabel *label = qobject_cast<QLabel *>(obj)) {
    label->setText(tr("Ping"));
} else if (QPushButton *button = qobject_cast<QPushButton *>(obj)) {
    button->setText(tr("Pong!"));
}
```

虽然可以在没有 Q_OBJECT 宏和没有元对象代码的情况下使用 QObject 作为基类，但如果不使用 Q_OBJECT 宏，则这里描述的信号和槽以及其他特性将不可用。从元对象系统的角度来看，没有元代码的 QObject 子类相当于其最接近的带有元对象代码的祖先。这意味着例如，QMetaObject::className() 将不会返回你的类的实际名称，而是返回这个祖先的类名。
因此，我们强烈建议所有 QObject 的子类无论是否真的使用信号、槽和属性都使用 Q_OBJECT 宏。