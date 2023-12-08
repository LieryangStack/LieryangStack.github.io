---
layout: post
title: 十七、GstStreams
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstStream基本概念

- GstStream对象是一个高级对象，定义了一个数据流，在GstPipeline中存在或可以存在。

- 它由一个唯一标识符“Stream ID”定义。一个GstStream并不自动意味着该流在管道或元素中存在。

- 能够在管道中引入新流的任何元素都应该创建适当的GstStream对象，并可以通过GST_EVENT_STREAM_START事件和/或GstStreamCollection传递该对象。

- 不修改流的性质的元素可以为其添加额外信息（例如丰富GstCaps或GstTagList）。这通常是由解析元素完成的。

## 1.1 GstStreams继承关系

```c
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstStream
```

## 2 GstStream类型结构

### 2.1 GstStream类型注册宏定义

```c
/* filename: gststreams.h */
#define GST_TYPE_STREAM             (gst_stream_get_type ())
/* filename: gststreams.c */
G_DEFINE_TYPE_WITH_CODE (GstStream, gst_stream, GST_TYPE_OBJECT,
    G_ADD_PRIVATE (GstStream) _do_init);
```

### 2.2 GstStream类型相关枚举

#### 2.2.1 GstStreamType

```c
typedef enum {
  /* 目前这个流没有被标识类型 */
  GST_STREAM_TYPE_UNKNOWN   = 1 << 0,
  /* 音频流 */
  GST_STREAM_TYPE_AUDIO     = 1 << 1,
  /* 视频流 */
  GST_STREAM_TYPE_VIDEO     = 1 << 2,
  /* 复用类型流 muxed container type */
  GST_STREAM_TYPE_CONTAINER = 1 << 3,
  /* 流包含字幕/图片数据 */
  GST_STREAM_TYPE_TEXT      = 1 << 4
} GstStreamType;
```

#### 2.2.2 GstStreamFlags
```c
/* filename: gstevent.h */
/**
 * GstStreamFlags:
 * @GST_STREAM_FLAG_NONE: 此流没有特殊属性
 * @GST_STREAM_FLAG_SPARSE: 此流是一个稀疏流（例如，字幕流），数据可能仅在不规则间隔内以较大间隔流动。
 * @GST_STREAM_FLAG_SELECT: 此流应默认选择。此标志可供解复用器使用，以表示在播放场景中应默认选择该流。
 * @GST_STREAM_FLAG_UNSELECT: 此流不应默认选择。此标志可供解复用器使用，以表示在播放场景中不应默认选择该流，但仅当用户显式选择时（例如，对于听障者的音频轨道或导演的解说音轨）。
 *
 * 自版本：1.2
 */
typedef enum {
  GST_STREAM_FLAG_NONE,
  GST_STREAM_FLAG_SPARSE       = (1 << 0),
  GST_STREAM_FLAG_SELECT       = (1 << 1),
  GST_STREAM_FLAG_UNSELECT     = (1 << 2)
} GstStreamFlags;
```

### 2.3 GstStream相关结构体

#### 2.3.1 GstStreamClass

```c
/* filename: gststreams.h */
struct _GstStreamClass {
  GstObjectClass parent_class;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.2 GstStream

```c
/* filename: gststreams.h */
/**
 * GstStream:
 * @stream_id: 此 #GstStream 的流标识符
 * 
 * 代表单个流的高级对象。它可能由管道中的实际数据流（#GstPad）支持，也可能没有。
 *
 * 一个 #GstStream 不关心数据的变化（例如解码、编码、解析等），只要底层数据流对应于相同的高级流（例如，特定的音频轨道）。
 *
 * 一个 #GstStream 包含有关流的所有相关信息，如流标识符、标签、能力、类型等。
 *
 * 元素可以为内部使用（包含与数据流相关的信息）子类化 #GstStream。
 *
 *
 * Since: 1.10
 */
struct _GstStream {
  /*< private >*/
  GstObject object;

  /*< public >*/
  const gchar *stream_id; /* 此 #GstStream的流标识符 */

  /*< private >*/
  GstStreamPrivate *priv;

  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.3 GstStreamPrivate

```c
/* filename: gststreams.c */
struct _GstStreamPrivate
{
  GstStreamFlags flags;
  GstStreamType type;
  GstTagList *tags;
  GstCaps *caps;
};
```

## 3 GstStream对象相关函数

