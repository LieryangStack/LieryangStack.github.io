---
layout: post
title: 二、Qt6笔记——信号与槽
categories: Qt6开发学习
tags: [Qt6开发学习]
---

## 1 信号与槽函数连接connect

这部分内容参考： `Qt6 C++开发指南` （3.3.4 信号与槽章节）

![alt text](/assets\Qt6\qt6_02_Signal_Slot\image\image.png)

查看代码源文件，可以发现有三种信号与槽连接函数声明。

### 1.1 Qt4版本通过名称连接

第一种就是Qt4版本中使用的，通过信号和槽函数名称连接，这种方式不会对信号和槽函数类型进行检查是否匹配。

```c++
/* 不带参数 */
connect(sender, SIGNAL(signal()), receiver, SLOT(slot()));

/* 带参数 */
connect(spinNum, SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));
```

### 1.2 Qt5版本通过函数指针

对于具有默认参数的信号，即信号名称是唯一的（不存在函数重载），不存在参数不同的其他同名的信号，可以使用这种函数指针形式进行关联，如：

```c++
connect(lineEdit, &QLineEdit::textChanged, this, &Widget::do_textChanged);
```

如果信号和槽函数发生重载,比如在窗口类 Widget 里设计了如下的两个自定义槽函数：

```c++
/* Widget自定了的两个槽函数 */
void do_click(bool checked); 
void do_click( );
```

我们就需要使用 `qOverload` 模板指定函数参数的类型 `QOverload<参数1类型，参数2类型，...>`

```c++
connect(ui->checkBox, &QCheckBox::clicked, this, qOverload<bool>(&Widget::do_click)); 
connect(ui->checkBox, &QCheckBox::clicked, this, qOverload<>(&Widget::do_click));

/* 或者 */
connect(ui->checkBox, qOverload<bool>(&QCheckBox::clicked), this, &Widget::do_click); 

/* 或者使用 QOverload */
connect(ui->checkBox, QOverload<bool>::of(&QCheckBox::clicked), this, &Widget::do_click);
```

### 1.3 不需要指定接受者

没有表示接收者的参数，接收者就是对象自身。例如，使用静态函数 connect()设
置连接的一条语句如下：

```c++
connect(spinNum, SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));
```

this 表示窗口对象，updateStatus()是窗口类里定义的一个槽函数。如果使用成员函数 connect()，就可以写成如下的语句：

```c++
this->connect(spinNum, SIGNAL(valueChanged(int)), SLOT(updateStatus(int)));
```

