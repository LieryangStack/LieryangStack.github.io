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

#### 2.2.1 GstMessageType
程序只会在播放状态下收到此消息，并且每次将处于 EOS 状态的管道设置为播放时。应用程序可以在管道中执行刷新 seek 操作，以撤消 EOS 状态。
```c
typedef enum {
  /* 未定义的消息 */
  GST_MESSAGE_UNKNOWN           = 0,
  /* 在管道中 end-of-stream到达。应用程序只会在播放状态下接收到此消息，并且每当将处于 EOS（结束）状态的管道设置为播放状态时，都会接收到该消息。
   * 应用程序可以在管道中执行flushing seek 操作，以撤消 EOS 状态。
   */
  GST_MESSAGE_EOS               = (1 << 0),
  /**
   * 发生错误。当应用程序收到错误消息时，应停止管道的播放，并不要假设会继续播放更多数据。
   * 可以通过将“redirect-location”字段设置到错误消息中来指定错误消息的重定向 URL。应用程序或高级别容器bin可能会根据需要使用此信息。
  */
  GST_MESSAGE_ERROR             = (1 << 1),
  /* 发生警告 */
  GST_MESSAGE_WARNING           = (1 << 2),
  /* 一个info消息 */
  GST_MESSAGE_INFO              = (1 << 3),
  /* tag消息 */
  GST_MESSAGE_TAG               = (1 << 4),
  /**
   * 管道正在缓冲。当应用程序在非实时管道的播放状态下收到缓冲消息时，必须暂停管道，直到缓冲完成(当消息中的百分比字段为 100% 时）。
   * 对于实时管道，无需执行任何操作，可以使用缓冲百分比来通知用户进度。
  */
  GST_MESSAGE_BUFFERING         = (1 << 5),
  /* 状态变化发生 */
  GST_MESSAGE_STATE_CHANGED     = (1 << 6),
  /* 此消息类型已经不再使用！！！。streaming线程一个元素状态发生变化。 */
  GST_MESSAGE_STATE_DIRTY       = (1 << 7),
  /* stepping操作完成 */
  GST_MESSAGE_STEP_DONE         = (1 << 8),
  /* 元素通知其提供时钟的能力。此消息仅在内部使用，不会转发给应用程序。 */
  GST_MESSAGE_CLOCK_PROVIDE     = (1 << 9),
  GST_MESSAGE_CLOCK_LOST        = (1 << 10),
  GST_MESSAGE_NEW_CLOCK         = (1 << 11),
  GST_MESSAGE_STRUCTURE_CHANGE  = (1 << 12),
  GST_MESSAGE_STREAM_STATUS     = (1 << 13),
  GST_MESSAGE_APPLICATION       = (1 << 14),
  GST_MESSAGE_ELEMENT           = (1 << 15),
  GST_MESSAGE_SEGMENT_START     = (1 << 16),
  GST_MESSAGE_SEGMENT_DONE      = (1 << 17),
  GST_MESSAGE_DURATION_CHANGED  = (1 << 18),
  GST_MESSAGE_LATENCY           = (1 << 19),
  GST_MESSAGE_ASYNC_START       = (1 << 20),
  GST_MESSAGE_ASYNC_DONE        = (1 << 21),
  GST_MESSAGE_REQUEST_STATE     = (1 << 22),
  GST_MESSAGE_STEP_START        = (1 << 23),
  GST_MESSAGE_QOS               = (1 << 24),
  GST_MESSAGE_PROGRESS          = (1 << 25),
  GST_MESSAGE_TOC               = (1 << 26),
  GST_MESSAGE_RESET_TIME        = (1 << 27),
  GST_MESSAGE_STREAM_START      = (1 << 28),
  GST_MESSAGE_NEED_CONTEXT      = (1 << 29),
  GST_MESSAGE_HAVE_CONTEXT      = (1 << 30),
  GST_MESSAGE_EXTENDED          = (gint) (1u << 31),
  GST_MESSAGE_DEVICE_ADDED      = GST_MESSAGE_EXTENDED + 1,
  GST_MESSAGE_DEVICE_REMOVED    = GST_MESSAGE_EXTENDED + 2,
  GST_MESSAGE_PROPERTY_NOTIFY   = GST_MESSAGE_EXTENDED + 3,
  GST_MESSAGE_STREAM_COLLECTION = GST_MESSAGE_EXTENDED + 4,
  GST_MESSAGE_STREAMS_SELECTED  = GST_MESSAGE_EXTENDED + 5,
  GST_MESSAGE_REDIRECT          = GST_MESSAGE_EXTENDED + 6,
  GST_MESSAGE_DEVICE_CHANGED    = GST_MESSAGE_EXTENDED + 7,
  GST_MESSAGE_INSTANT_RATE_REQUEST = GST_MESSAGE_EXTENDED + 8,
  GST_MESSAGE_ANY               = (gint) (0xffffffff)
} GstMessageType;
```

### 2.3 GstMessage相关结构体

## 3 GstMessage对象相关函数