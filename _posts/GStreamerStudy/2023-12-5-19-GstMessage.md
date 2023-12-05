---
layout: post
title: 十九、GstMessage
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstMessage基本概念

- 消息是继承于 GstMiniObject 的子类，其内容是一个通用的 GstStructure。这样做可以编写自定义消息而无需进行 API 更改，同时允许各种不同类型的消息。

- 消息由管道中的对象发布，并通过 GstBus 传递给应用程序。

- 在 GstBus 上发布消息的基本使用模式如下：
  ```c
  gst_bus_post (bus, gst_message_new_eos());
  ```

- 通常，GstElement 使用 gst_element_post_message 在其父容器提供的总线上发布消息。

## 2 GstMessage类型结构

### 2.1 GstMessage类型注册宏定义

### 2.2 GstMessage类型相关枚举

### 2.3 GstMessage相关结构体

## 3 GstMessage对象相关函数