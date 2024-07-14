---
layout: post
title: 三、GTK4核心对象——GdkGLContext
categories: GTK4核心对象
tags: [GTK4核心对象]
---

## 1 GdkGLContext

![alt text](/assets/GTK4/GTK4Core/03_GLContext/image/image-7.png)

创建 `GdkGLContext` 时候，一般都需要指定 `GdkSurface`

![alt text](/assets/GTK4/GTK4Core/03_GLContext/image/image-9.png)

`GdkGLContext` 中含有 `egl_context`（egl上下文是在渲染的时候创建）

![alt text](/assets/GTK4/GTK4Core/03_GLContext/image/image-8.png)

## 2 GdkGLContext创建分析

1. `GdkX11Display` 创建了 `GdkX11GLContextEGL`，整个程序其他的 `GdkGLContext` 都是基于默认显示的上下文创建。

    ![alt text](/assets/GTK4/GTK4Core/03_GLContext/image/image-10.png)
  
2. `GtkWindow` 创建了 `GdkSurface`，程序运行过程中并没有创建 `GdkSurface->gl_paint_context`（GdkGLContext）。

3. `GskRenderer` 创建了 `GdkGLContext`。基于默认显示的上下文，创建的。(这里并不是什么共享的GdkGLContext，仅仅是获取第一步创建的GdkGLContext类型，渲染时候创建egl_context，才是基于第一步创建的egl，创建egl共享上下文)。

    ![alt text](/assets/GTK4/GTK4Core/03_GLContext/image/image-11.png)

## 3 GdkGLContext创建方式

第一次创建 gdk_x11_display_init_gl

其他共享创建 gdk_display_create_gl_context
            
           gdk_surface_create_gl_context