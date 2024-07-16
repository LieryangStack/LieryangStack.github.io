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

`gl_paint_context` 一般都是通过默认显示里面的glx_contxt，通过 `gdk_surface_create_gl_context` 创建一个共享的 gl_paint_context。

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-5.png)



### 1.1 X11实现GdkSurface

- **GdkX11DragSurface**：默认显示会创建该surface，这个surface（也就是这个窗口）是在拖拽的时候是用。

参考： Clipboard

- **GdkX11Toplevel**：

- **GdkX11Popup**：

![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-1.png)

#### 1.1.1 GdkX11DragSurface

1. 默认显示 `GdkX11Display` 中创建了一个 `GdkX11DragSurface`。

    ![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-2.png)

#### 1.1.2 GdkX11Toplevel

2. `GtkWindow` 中创建了一个 `GdkX11Toplevel`。

    ![alt text](/assets/GTK4/GTK4Core/02_Surface/image/image-3.png)

#### 1.1.3 GdkX11Popup




3. GdkSurface中的egl_surface，还有其中GdkGLContext的egl_context，是在渲染阶段才会创建
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





