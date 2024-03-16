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

&emsp;

![Alt text](/assets/GStreamerStudy/CoreObject/19_GstMessage/communication.png)

## 2 GstMessage类型结构

### 2.1 GstMessage类型注册宏定义

```c
/* filename: gstmessage.h */
GST_API GType _gst_message_type;
#define GST_TYPE_MESSAGE                         (_gst_message_type)
/* filename: gstmessage.c */
GType _gst_message_type = 0;
GST_DEFINE_MINI_OBJECT_TYPE (GstMessage, gst_message);
```

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
  /**
   * 管道当前选择的时钟变得无法使用。管道将在下一个播放状态更改时选择一个新的时钟。
   * 应用程序应在收到此消息时将管道设置为暂停状态，然后再设置为播放状态。
  */
  GST_MESSAGE_CLOCK_LOST        = (1 << 10),
  /* 管道选择了一个新时钟 */
  GST_MESSAGE_NEW_CLOCK         = (1 << 11),
  /* 管道结构发生了变化。此消息仅在内部使用，不会转发给应用程序。 */
  GST_MESSAGE_STRUCTURE_CHANGE  = (1 << 12),
  /* 关于流的状态，当它开始、停止、出错等时发出。 */
  GST_MESSAGE_STREAM_STATUS     = (1 << 13),
  /* 由应用程序发布的消息，可能通过特定于应用程序的元素发布。 */
  GST_MESSAGE_APPLICATION       = (1 << 14),
  /* 元素特定的消息，请参阅特定元素的文档。 */
  GST_MESSAGE_ELEMENT           = (1 << 15),
  /* 管道开始播放一个段。此消息仅在内部使用，不会转发给应用程序。 */
  GST_MESSAGE_SEGMENT_START     = (1 << 16),
  /* 管道完成播放一个段。此消息在所有先发布了 `GST_MESSAGE_SEGMENT_START` 的元素，
   * 然后该元素发布了 `GST_MESSAGE_SEGMENT_DONE` 消息后转发给应用程序。 */
  GST_MESSAGE_SEGMENT_DONE      = (1 << 17),
  /* 管道的持续时间发生了变化。应用程序可以通过持续时间查询获取新的持续时间。 */
  GST_MESSAGE_DURATION_CHANGED  = (1 << 18),
  /* 元素的延迟发生变化时发布。应用程序应重新计算并分发新的延迟。 */
  GST_MESSAGE_LATENCY           = (1 << 19),
  /* 元素开始异步 #GstStateChange 时发布。此消息不会转发给应用程序，而是在内部使用。 */
  GST_MESSAGE_ASYNC_START       = (1 << 20),
  /* 元素完成异步 #GstStateChange 时发布。应用程序只会从顶层管道接收到此消息。 */
  GST_MESSAGE_ASYNC_DONE        = (1 << 21),
  /* 元素希望管道更改状态时发布。此消息是对应用程序的建议，应用程序可以决定对（部分）管道执行状态更改。 */
  GST_MESSAGE_REQUEST_STATE     = (1 << 22),
  /* 一个stepping操作被开始 */
  GST_MESSAGE_STEP_START        = (1 << 23),
  /* 由于服务质量原因，缓冲区被丢弃或元素更改了其处理策略。 */
  GST_MESSAGE_QOS               = (1 << 24),
  /* 一个进度消息。 */
  GST_MESSAGE_PROGRESS          = (1 << 25),
  /* 发现了一个新的目录（table of contents），或者之前发现的 TOC 已更新。 */
  GST_MESSAGE_TOC               = (1 << 26),
  /* 请求重置管道的运行时间。这是一个内部消息，应用程序可能永远不会收到。 */
  GST_MESSAGE_RESET_TIME        = (1 << 27),
  /* 指示新流的开始。例如，在使用 gapless 播放模式时，当下一个标题实际开始播放时通知（在设置下一个标题的 URI 后的一段时间内）。 */
  GST_MESSAGE_STREAM_START      = (1 << 28),
  /* 指示元素需要特定上下文的消息（自 1.2 版本开始）。 */
  GST_MESSAGE_NEED_CONTEXT      = (1 << 29),
  /* 指示元素创建了一个上下文的消息（自 1.2 版本开始）。 */
  GST_MESSAGE_HAVE_CONTEXT      = (1 << 30),
  /* 消息是扩展消息类型（见下文）。这些扩展消息 ID 不能直接与基于掩码的 API（例如 gst_bus_poll() 或 gst_bus_timed_pop_filtered()）一起使用，
   * 但仍然可以对 `GST_MESSAGE_EXTENDED` 进行过滤，然后检查特定类型的结果（自 1.4 版本开始）。 */
  GST_MESSAGE_EXTENDED          = (gint) (1u << 31),
  /* 指示一个 #GstDevice 被添加到 #GstDeviceProvider（自 1.4 版本开始）。 */
  GST_MESSAGE_DEVICE_ADDED      = GST_MESSAGE_EXTENDED + 1,
  /* 指示一个 #GstDevice 从 #GstDeviceProvider 中被移除（自 1.4 版本开始）。 */
  GST_MESSAGE_DEVICE_REMOVED    = GST_MESSAGE_EXTENDED + 2,
  /* 指示一个 #GObject 属性已更改（自 1.10 版本开始）。 */
  GST_MESSAGE_PROPERTY_NOTIFY   = GST_MESSAGE_EXTENDED + 3,
  /* 指示有一个新的 #GstStreamCollection 可用（自 1.10 版本开始）。 */
  GST_MESSAGE_STREAM_COLLECTION = GST_MESSAGE_EXTENDED + 4,
  /* 指示 #GstStreams 的活动选择已更改（自 1.10 版本开始）。 */
  GST_MESSAGE_STREAMS_SELECTED  = GST_MESSAGE_EXTENDED + 5,
  /* 指示请求应用程序尝试播放给定的 URL。例如，如果接收到带有非 HTTP URL 的 HTTP 302/303 响应，则此消息很有用。 */
  GST_MESSAGE_REDIRECT          = GST_MESSAGE_EXTENDED + 6,
  /* 指示 #GstDevice 在 #GstDeviceProvider 中发生了变化。 */
  GST_MESSAGE_DEVICE_CHANGED    = GST_MESSAGE_EXTENDED + 7,
  /* 由元素发送的消息，请求从管道获取运行时间，以便在应用即时速率变化时应用（当答复到达时可能已经是过去的情况）。 */
  GST_MESSAGE_INSTANT_RATE_REQUEST = GST_MESSAGE_EXTENDED + 8,
  /* 所有上述消息的掩码 */
  GST_MESSAGE_ANY               = (gint) (0xffffffff)
} GstMessageType;
```

#### 2.2.2 GstStructureChangeType

```c
typedef enum {
  GST_STRUCTURE_CHANGE_TYPE_PAD_LINK   = 0,  // Pad 链接开始或完成
  GST_STRUCTURE_CHANGE_TYPE_PAD_UNLINK = 1   // Pad 取消链接开始或完成
} GstStructureChangeType;
```

#### 2.2.3 GstStreamStatusType

```c
/* %GST_MESSAGE_STREAM_STATUS 的类型。流状态消息通知应用程序新的流线程及其状态。 */
typedef enum {
  GST_STREAM_STATUS_TYPE_CREATE   = 0,   // 需要创建一个新线程
  GST_STREAM_STATUS_TYPE_ENTER    = 1,   // 一个线程进入其循环函数
  GST_STREAM_STATUS_TYPE_LEAVE    = 2,   // 一个线程离开其循环函数
  GST_STREAM_STATUS_TYPE_DESTROY  = 3,   // 一个线程被销毁

  GST_STREAM_STATUS_TYPE_START    = 8,   // 一个线程已启动
  GST_STREAM_STATUS_TYPE_PAUSE    = 9,   // 一个线程被暂停
  GST_STREAM_STATUS_TYPE_STOP     = 10   // 一个线程已停止
} GstStreamStatusType;
```

#### 2.2.4 GstProgressType

```c
 /* %GST_MESSAGE_PROGRESS 的类型。进度消息通知应用程序异步任务的状态。*/
typedef enum {
  GST_PROGRESS_TYPE_START    = 0,   // 开始一个新任务
  GST_PROGRESS_TYPE_CONTINUE = 1,   // 一个任务完成，另一个任务正在继续
  GST_PROGRESS_TYPE_COMPLETE = 2,   // 一个任务完成
  GST_PROGRESS_TYPE_CANCELED = 3,   // 一个任务被取消
  GST_PROGRESS_TYPE_ERROR    = 4    // 一个任务引发了错误
} GstProgressType;

```


### 2.3 GstMessage相关结构体

#### 2.3.1 GstMessage

```c
/* filename: gstmessage.h */
struct _GstMessage
{
  GstMiniObject   mini_object;

  /*< public > *//* with COW */
  GstMessageType  type;  /* 消息的类型 */
  guint64         timestamp; /* 消息的时间戳 */
  GstObject      *src; /* 发布该消息的源头 */
  guint32         seqnum; /* 消息的序列号 */

  /*< private >*//* with MESSAGE_LOCK */
  GMutex          lock;  /* lock and cond for async delivery */
  GCond           cond;
};
```

#### 2.3.2 GstMessageImpl

```c
/* filename: gstmessage.c */
typedef struct
{
  GstMessage message;

  GstStructure *structure;
} GstMessageImpl;
```

#### 2.3.3 GstMessageQuarks

```c
/* filename: gstmessage.c */
typedef struct
{
  const gint type; /* 对应枚举 GstMessageType */
  const gchar *name; /* 消息类型字符串表达，例如： "unknown", "eos"等等 */
  GQuark quark; /* name对应的Quark */
} GstMessageQuarks;
```

## 3 GstMessage对象相关函数