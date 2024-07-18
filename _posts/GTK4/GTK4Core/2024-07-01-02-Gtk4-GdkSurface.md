---
layout: post
title: 二、GTK4核心对象——GdkSurface
categories: GTK4核心对象
tags: [GTK4核心对象]
---

## 1 GdkSurface

1. GdkSurface创建窗口。比如GdkX11Surface中的 `gdk_x11_surface_create_window` 函数，调用 `XCreateWindow` 来创建窗口。

2. GdkSurface管理窗口属性，`x11_surface_resize ()` 函数，调用 `XResizeWindow()` 来修改窗口的大小size

3. GdkSurface显示窗口，`gdk_x11_surface_show ()` 函数，调用 `XMapWindow()` 来显示窗口。

4. GdkSurface中含有 `egl_surface` 成员。

### 1.1 GdkSurface有那些成员

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-4.png)

#### 1.1.1 成员GdkGLContext和EGLSurface

`gl_paint_context` 一般都是通过默认显示里面的glx_contxt，通过 `gdk_surface_create_gl_context` 创建一个共享的 gl_paint_context。

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-5.png)

#### 1.1.2 成员GdkFrameClock



### 1.2 X11实现GdkSurface

- **GdkX11DragSurface**：默认显示GdkX11Display会创建该surface（这个surface也就是一个X窗口），该窗口是在拖拽的时候是使用。

    参考： gtk4-demo 中的 Clipboard 示例。拖拽浮动的颜色控件其实就是DragSurface窗口。

    ![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-7.png)

- **GdkX11Toplevel**：GtkWindow会创建该surface，这就是程序的主窗口（可以创建多个）。

- **GdkX11Popup**：右击窗口、菜单弹出窗口、工具窗口、下拉弹出窗口等都是popup类的surface窗口。
    ![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-8.png)

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-1.png)

#### 1.2.1 GdkX11DragSurface

默认显示 `GdkX11Display` 中创建了一个 `GdkX11DragSurface`。也就是拖拽Widget的时候被拖拽的那个窗口。

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-2.png)

#### 1.2.2 GdkX11Toplevel

`GtkWindow` 中创建了一个 `GdkX11Toplevel`。也就是GtkWindow那个窗口。

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-3.png)

#### 1.2.3 GdkX11Popup

`GtkPopover` 、 `GtkTextHandle` 、 `GtkTooltipWindow` 都会创建该surface，其实就是那个下拉、菜单或者右击的弹出窗口。

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-9.png)


## 2 GdkSurface程序分析

GdkSurface中的egl_surface，还有其中GdkGLContext的egl_context，是在渲染阶段才会创建
    ![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-6.png)

<font color="red">
注意：
</font>


<font color="red">
1. GdkDisplay连接到显示器，每个程序一般只有一个，GdkDisplay不属于Widget，跟Widget其实没有多大关系，GdkDisplay表示的是连接到显示服务器。
</font>

<font color="red">
2. GdkSurface跟每GtkNative有关、每个实现GdkNative接口的构件（比如GtkWindow、GtkPopover）都会有一个GdkSurface。这些构建（比如GtkWindow、GtkPopover）内部的子构件是同一个GdkSurface。
</font>





