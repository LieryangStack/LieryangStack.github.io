---
layout: post
title: 四、GstAllocator
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 基本概念

`GstAllocator` 是用来创建内存 `GstMemory`。

内存（GstMemory）通常是通过分配器使用 `gst_allocator_alloc` 方法调用创建的。当使用 NULL 作为分配器时，将使用默认的分配器。

可以通过 `gst_allocator_register` 注册新的分配器。分配器通过名称来识别，并且可以通过 `gst_allocator_find` 检索。可以使用 `gst_allocator_set_default` 来更改默认分配器。

可以使用 `gst_memory_new_wrapped` 创建新的内存，该方法包装在其他地方分配的内存。

## 2 继承关系

```c
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstAllocator
```