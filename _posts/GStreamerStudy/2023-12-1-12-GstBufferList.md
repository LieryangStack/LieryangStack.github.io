---
layout: post
title: 十二、GstBufferList
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstBufferList基本概念

- 缓冲列表（Buffer lists）是包含一系列缓冲（buffers）的对象。

- 缓冲列表可以通过 gst_buffer_list_new 创建，并使用 gst_buffer_list_insert 填充数据。

- 当需要一次性推送多个缓冲时，可以使用 gst_pad_push_list 将缓冲列表推送到源端口（srcpad）。这在需要推送多个缓冲时非常有用，因为它可以减少逐个推送每个缓冲时的开销。

- <span style="color:red;">GstBufferList中的buffer需要用户自己申请内存空间，然后使用 gst_buffer_list_insert 插入到列表中</span>

## 2 GstBufferList类型结构

### 2.1 GstBufferList类型注册宏定义
```c
/* filename:  gstbufferlist.h   */
GST_API GType _gst_buffer_list_type;
#define GST_TYPE_BUFFER_LIST      (_gst_buffer_list_type)

/* filename:  gstbufferlist.c   */
GType _gst_buffer_list_type = 0;

GST_DEFINE_MINI_OBJECT_TYPE (GstBufferList, gst_buffer_list);
```

### 2.2 GstBufferList结构体

```c
/* filename:  gstbufferlist.c   */

struct _GstBufferList
{
  GstMiniObject mini_object;

  GstBuffer **buffers;
  guint n_buffers; /* 目前列表存储有多少个buffer */
  guint n_allocated; /* 列表可以存储多少个buffer（分配了多少个buffer指针存储） */

  gsize slice_size; /* sizeof(GstBufferList) + ( n_allocated - 1 ) * sizeof(void *) */

  /* 虽然这里的GstBuffer指针数组只能存储一个，但是初始化函数分配了sizeof（GstBufferList）+ 存储指针的连续空间 
   * 意味着 arr[2] arr[3] .... arr[n_allocated] 都有效
   * @note: sizeof（GstBufferList） = 96，这个结构体的最后一个成员 arr是占用 88字节，所以不会出现结构体内存对齐问题
   */
  GstBuffer *arr[1];
};
```

## 3 GstBufferList对象相关函数