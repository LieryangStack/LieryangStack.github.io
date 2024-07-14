---
layout: post
title: 一、GTK4核心对象——GdkDisplay
categories: GTK4核心对象
tags: [GTK4核心对象]
---

- GdkDisplayManager 和 GdkDisplay 创建都是在 `gtk_init()` 函数调用的，而且一个程序一般只有一个 GdkDisplayManager 和 GdkDisplay

## 1 GdkDisplayManager

`GdkDisplayManager` 对象可以打开一个默认的 `GdkDisplay`对象和存储多个 `GdkDisplay` 对象。 

1. 通过 `gdk_display_manager_get()` 函数，可以创建或者获取到唯一的静态 `GdkDisplayManager` 对象。

    ![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-7.png)

2. 通过 `gdk_display_manager_open_display()` 函数和设置 `GDK_BACKEND` 环境变量，可以打开一个显示后端，设置默认的 `default_display` ，并且创建，然后返回 `GdkDisplay` 对象。
    ![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-8.png)

3. 通过环境变量 `GDK_BACKEND` ，可以选择打开不同的显示后端。

   ![alt text](/assets/GTK4/GTK4Core/01_Display/image/image.png)


## 2 GdkDisplay

1. `GdkX11Display` 继承于 `GdkDisplay`。

2. `GdkX11Display` 是对 X11显示的一个封装对象。

3. GdkDisplayManager和GdkDisplay整个程序一般只有一个，GdkDisplayManager通过静态变量存储，GdkDisplay存储到GdkDisplayManager种。

4. `gdk_display_open (NULL)` 调用了 `XOpenDisplay(NULL)` 函数，并且创建了 `GdkX11Display` 对象。

    ![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-1.png)

5. `GdkDisplay` 含有 `egl_display` 和 `egl_config` 对象。

### 2.1 GdkDisplay函数总结

#### 2.1.1 获取GdkDisplay对象

```c
GdkDisplay *
gdk_display_get_default (void); /* 程序默认显示对象 */

GdkDisplay *
gdk_surface_get_display (GdkSurface *surface);

GdkDisplay *
gtk_widget_get_display (GtkWidget *widget);
```

#### 2.1.2 从GdkDisplay获取egl_display、egl_config

```c
gpointer
gdk_display_get_egl_config (GdkDisplay *self);

gpointer
gdk_display_get_egl_display (GdkDisplay *self)
```

## 3 程序分析

### 3.1 创建GdkDisplayManager、GdkDisplay

![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-9.png)

"open"信号的默认处理函数 `gdk_display_real_opened` 会调用 `_gdk_display_manager_add_display` 函数。

![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-10.png)

### 3.2 初始gl的时候创建GdkSurface、GdkGLContext

依次创建GdkDisplayManager、GdkDisplay完成后，会在最后初始化gl，然后依次创建 GdkSurface、GdkGLContext。

![alt text](/assets/GTK4/GTK4Core/01_Display/image/image-11.png)