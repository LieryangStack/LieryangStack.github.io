---
layout: post
title: 四、GTK4核心对象——GdkFrameClock
categories: GTK4核心对象
tags: [GTK4核心对象]
---

## 1 GdkFrameClock

- GdkFrameClock告诉应用程序何时更新和重新绘制表面。比如：

  - show函数的时候，调用 gdk_frame_clock_begin_updating 函数，从而注册空闲绘制或者空闲刷新函数。
  
  - 接受到鼠标进入窗口事件，事件源就会注册GdkFrameClock空闲刷新函数。

    ![alt text](/assets/GTK4/GTK4Core/04_GdkFrameClock/image/image.png)

## 2 创建GdkFrameClock对象

`GdkSurface` 中含有该对象成员 `GdkFrameClock`。

![alt text](/assets/GTK4/GTK4Core/04_GdkFrameClock/image/image-1.png)

创建surface的时候，会调用GdkX11TopLevel或者GdkX11DragSurface对象实现的constructed函数，创建GdkFrameClock对象。

![alt text](/assets/GTK4/GTK4Core/04_GdkFrameClock/image/image-2.png)

## 3 调用GdkFrameClock空闲刷新或者绘制函数分析

![alt text](/assets/GTK4/GTK4Core/04_GdkFrameClock/image/image-3.png)
