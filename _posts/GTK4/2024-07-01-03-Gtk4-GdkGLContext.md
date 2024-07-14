---
layout: post
title: 三、GTK4核心对象——GdkGLContext
categories: GTK4核心对象
tags: [GTK4核心对象]
---


## 三种创建

第一次创建 gdk_x11_display_init_gl

其他共享创建 gdk_display_create_gl_context
            
           gdk_surface_create_gl_context