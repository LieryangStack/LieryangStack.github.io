---
layout: post
title: 十四、GstContext
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstContext基本概念

- GstContext 是一个容器对象，用于存储诸如设备上下文、显示服务器连接以及应在多个元素之间共享的类似概念。

- 应用程序可以通过使用 gst_element_set_context 在整个管道上设置上下文，然后该上下文将被传播到所有子元素。元素可以在 set_context 中处理这些上下文，并将它们与已有的上下文信息合并。

- 当一个元素需要一个上下文时，它将按以下顺序执行以下操作，直到其中一个步骤成功：
  
  1. 检查元素是否已经有一个上下文。
  2. 使用 GST_QUERY_CONTEXT 向下游查询上下文。
  3. 使用 GST_QUERY_CONTEXT 向上游查询上下文。
  4. 在总线上发布一个 GST_MESSAGE_NEED_CONTEXT 消息，消息中包含所需的上下文类型，然后检查是否现在已经设置了一个可用的上下文。
  5. 自行创建一个上下文，并在总线上发布一个 GST_MESSAGE_HAVE_CONTEXT 消息。

二进制文件（Bins）将捕获 GST_MESSAGE_NEED_CONTEXT 消息，并且如果可能的话，会在请求它的元素上设置任何以前已知的上下文。否则，如果应用程序可以提供一个上下文，它应该这样做。

GstContext 可以是持久的。一个持久的 GstContext 在元素达到 GST_STATE_NULL 时会被保留，非持久的上下文将被移除。此外，非持久的上下文不会覆盖之前设置到元素上的持久上下文。

## 2 GstContext类型结构

### 2.1 GstContext类型注册宏定义

```c
/* filename: gstcontext.h */
GST_API GType _gst_context_type;
#define GST_TYPE_CONTEXT                         (_gst_context_type)

GST_API
GType           gst_context_get_type            (void);
/* filename: gstcontext.c */
GType _gst_context_type = 0;
GST_DEFINE_MINI_OBJECT_TYPE (GstContext, gst_context);
```

### 2.2 GstContext结构体

```c
/* filename: gstcontext.c */
struct _GstContext
{
  GstMiniObject mini_object;

  gchar *context_type; /* 上下文类型名称 */
  GstStructure *structure;
  gboolean persistent; /* 是否是持久性上下文 */
};
```

## 3 GstContext对象相关函数