---
layout: post
title: 十一、GTK4——GdkDisplay、GtkNative、GdkSurface
categories: GTK4
tags: [GTK4]
---

## 1 进程中一般只存在一种的对象

### 1.1 GdkDisplayManager

`GdkDisplayManager` 对象可以打开一个默认的 `GdkDisplay`对象和存储多个 `GdkDisplay` 对象。

1. 通过 `gdk_display_manager_get()` 函数，可以创建或者获取到唯一的静态 `GdkDisplayManager` 对象。

2. 通过 `gdk_display_manager_open_display()` 函数和设置 `GDK_BACKEND` 环境变量，可以打开一个显示后端，设置默认的 `default_display` ，并且创建，然后返回 `GdkDisplay` 对象。

3. 创建静态 `GdkDisplayManager` 对象和打开默认显示，都是在 `gtk_init()` 函数中完成。

4. 通过环境变量 `GDK_BACKEND` ，可以选择打开不同的显示后端。

   ![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image.png)


### 1.2 GdkDisplay

1. `GdkX11Display` 继承于 `GdkDisplay`。

2. `GdkX11Display` 是对 X11显示的一个封装对象。

3. GdkDisplayManager和GdkDisplay整个程序一般只有一个，GdkDisplayManager通过静态变量存储，GdkDisplay存储到GdkDisplayManager种。

4. `gdk_display_open (NULL)` 调用了 `XOpenDisplay(NULL)` 函数，并且创建了 `GdkX11Display` 对象。

    ![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image-1.png)

## 2 GtkWindow窗口（每个窗口都会创建，但是窗口内只有一个种该对象）

### 2.1 GtkNative

1. GtkNative接口可以获取到窗口的 `GdkSurface` 和 `GskRenderer` 

2. 通过 `gtk_native_get_surface()` 函数，可以获取到 `GdkSurface`

3. 通过 `gtk_native_get_renderer()` 函数，可以获取到 `GskRenderer` 

意味着实现 `GtkNative` 接口，也必须有 `GdkSurface` 和 `GskRenderer` 成员。

![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image-2.png)

## 2.1.2 那些Widget实现了GtkNative接口

1. 一个窗口win内部的所有子Widget，都是同一个GtkNative，也就是同一个GdkSurface

2. 实现了GtkNative接口，也意味着都实现了GdkSurface创建。

### 2.2 GdkSurface

1. GdkSurface创建窗口。比如GdkX11Surface中的 `gdk_x11_surface_create_window` 函数，调用 `XCreateWindow` 来创建窗口。

2. GdkSurface管理窗口属性，`x11_surface_resize ()` 函数，调用 `XResizeWindow()` 来修改窗口的大小size

3. GdkSurface显示窗口，`gdk_x11_surface_show ()` 函数，调用 `XMapWindow()` 来显示窗口。
 
  ![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image-3.png)

## 3 分析程序

![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image-4.png)

1. gtk_init() 函数创建显示、连接到显示服务器。

2. gtk_window_present (win) 显示窗口。

    ![alt text](/assets/GTK4/11_GdkDisplay_GdkSurface/image/image-5.png)

<font color="red">
注意：

1. GdkDisplay连接到显示器，每个程序一般只有一个，GdkDisplay不属于Widget，跟Widget其实没有多大关系，GdkDisplay表示的是连接到显示服务器。

2. GdkSurface跟每GtkNative有关、每个实现GdkNative接口的构件（比如GtkWindow、GtkPopover）都会有一个GdkSurface。这些构建（比如GtkWindow、GtkPopover）内部的子构件是同一个GdkSurface。
</font>

## 4 总结

### 4.1 两种获取GdkDisplay的方式

```c
/* 1.通过GdkSurface获取GdkDisplay */

GtkNative *
gtk_widget_get_native (GtkWidget *widget); /* 先获取GtkNative */

GdkSurface *
gtk_native_get_surface (GtkNative *self);

/* 2.直接获取全局默认GdkDisplay */
GdkDisplay *
gdk_display_get_default (void);
```