---
layout: post
title: 十八、GstStreamCollection
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstStreamCollection基本概念

- 可获得GstStream的集合。

- GstStreamCollection将由可以提供这些流的元素提供。应用程序可以使用集合来向用户展示可用的流，使用 gst_stream_collection_get_stream()。

- 一旦发布，GstStreamCollection就是不可变的。更新是通过发送新的GstStreamCollection消息来完成的，这个新消息可能会或可能不会与它替换的集合共享一些GstStream对象。接收者可以检查流集合消息的发送者，以了解哪个集合已过时。

- 管道中的多个元素可以提供GstStreamCollection。

- 应用程序可以通过在管道、容器或元素上使用 GST_EVENT_SELECT_STREAMS 事件来激活集合中的流。

## 2 GstStreamCollection类型结构

### 2.1 GstStreamCollection类型注册宏定义

### 2.2 GstStreamCollection类型相关枚举

### 2.3 GstStreamCollection相关结构体

## 3 GstStreamCollection对象相关函数