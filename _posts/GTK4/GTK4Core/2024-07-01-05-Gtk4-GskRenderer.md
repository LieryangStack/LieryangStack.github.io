---
layout: post
title: 四、GTK4核心对象——GskRenderer
categories: GTK4核心对象
tags: [GTK4核心对象]
---

## 1 GskRenderer

### 1.1 GskRenderer继承关系

![alt text](/assets/GTK4/GTK4Core/05_GskRenderer/image/image.png)

### 1.2 GskRenderer成员

![alt text](/assets/GTK4/GTK4Core/05_GskRenderer/image/image-1.png)

### 1.3 创建GskRenderer

![alt text](/assets/GTK4/GTK4Core/05_GskRenderer/image/image-2.png)

### 1.4 GskRenderer创建GdkGLContext函数

通过surface里面的display获取到GdkGLContext类型，创建新的GdkGLContext

![alt text](/assets/GTK4/GTK4Core/05_GskRenderer/image/image-3.png)

### 1.5 GskGpuRenderer渲染函数

![alt text](/assets/GTK4/GTK4Core/05_GskRenderer/image/image-4.png)

