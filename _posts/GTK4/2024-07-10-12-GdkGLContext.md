---
layout: post
title: 十二、GTK4——GdkGLContext
categories: GTK4
tags: [GTK4]
---

1. GdkDisplay中创建GdkGLContext

2. GdkSurface中创建GdkGLContext

gtk_init() 初始化函数中要执行的内容

1. 先创建默认的GdkX11Display（继承于GdkDisplay）  这个Surface一般不需要刷新

2. 创建GdkX11DragSurface （继承于GdkSurface）

3. 最后创建GdkX11GLContextEGL（继承于GdkGLContext、GdkDrawContext）

gtk_window_present(win) 函数中调用执行的内容

1. 创建GdkX11Toplevel（继承于GdkSurface）    这个Surface一般需要刷新

2. 通过默认的GdkX11GLContextEGL 创建共享GdkX11GLContextEGL（继承于GdkGLContext、GdkDrawContext）