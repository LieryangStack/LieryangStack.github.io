---
layout: post
title: 二十三、GstElement
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstElement基本概念

- GstElement是构建可用于GStreamer管道的元素所需的<span style="color:red;">抽象基类（不可实例化）</span>。有关创建GstElement子类的更多信息，请参阅插件编写指南。

- 使用gst_element_get_name可以获取GstElement的名称，并使用gst_element_set_name进行设置。为了提高速度，在核心中使用GST_ELEMENT_NAME时，需要使用适当的锁。不要在插件或应用程序中使用此功能，以保留ABI兼容性。

- 元素可以拥有GstPad类型的pad。这些pad链接到其他元素上的pad。GstBuffer在这些链接的pad之间流动。GstElement具有GstPad结构的GList，用于其所有输入（或sink）和输出（或source）pad。核心和插件编写者可以使用gst_element_add_pad和gst_element_remove_pad来添加和删除pad。

- 可以使用gst_element_get_static_pad按名称检索元素的现有pad。可以使用gst_element_request_pad和GstPadTemplate创建一个新的动态pad。可以使用gst_element_iterate_pads获取所有pad的迭代器。

- 通过它们的pad可以将元素链接在一起。如果链接很简单，可以使用gst_element_link方便函数链接两个元素，或使用gst_element_link_many链接多个元素。使用gst_element_link_filtered按指定的一组GstCaps约束链接两个元素。为了更精细地控制，可以使用gst_element_link_pads和gst_element_link_pads_filtered按名称指定每个元素上要链接的pad。

- 每个元素都有一个状态（参见GstState）。可以使用gst_element_get_state和gst_element_set_state获取和设置元素的状态。设置状态会触发GstStateChange。要获取GstState的字符串表示形式，请使用gst_element_state_get_name。

- 可以使用gst_element_get_clock和gst_element_set_clock在元素上获取和设置GstClock。如果设置了GST_ELEMENT_FLAG_PROVIDE_CLOCK标志，一些元素可以为管道提供时钟。可以使用gst_element_provide_clock方法检索由这种元素提供的时钟。并非所有元素都需要时钟才能正确运行。如果设置了GST_ELEMENT_FLAG_REQUIRE_CLOCK()标志，则应该使用gst_element_set_clock在元素上设置时钟。

- 请注意，时钟的选择和分发通常由顶级的GstPipeline处理，因此时钟函数只能在非常特定的情况下使用。

## 2 GstElement类型结构

### 2.1 GstElement类型注册宏定义

```c
/* filename: gstelement.h */
#define GST_TYPE_ELEMENT                (gst_element_get_type ())

/* filename: gstelement.c */
gst_element_get_type (void)
{
  static gsize gst_element_type = 0;

  if (g_once_init_enter (&gst_element_type)) {
    GType _type;
    static const GTypeInfo element_info = {
      sizeof (GstElementClass),
      gst_element_base_class_init,
      NULL,                     /* base_class_finalize */
      (GClassInitFunc) gst_element_class_init,
      NULL,
      NULL,
      sizeof (GstElement),
      0,
      (GInstanceInitFunc) gst_element_init,
      NULL
    };

    _type = g_type_register_static (GST_TYPE_OBJECT, "GstElement",
        &element_info, G_TYPE_FLAG_ABSTRACT);

    __gst_elementclass_factory =
        g_quark_from_static_string ("GST_ELEMENTCLASS_FACTORY");
    __gst_elementclass_skip_doc =
        g_quark_from_static_string ("GST_ELEMENTCLASS_SKIP_DOCUMENTATION");
    g_once_init_leave (&gst_element_type, _type);
  }
  return gst_element_type;
}
```


### 2.2 GstElement类型相关枚举

#### 2.2.1 GstState

```c
/* gstelement.h */

/**
 * 元素可能处于的状态。可以使用gst_element_set_state()更改状态，并使用gst_element_get_state()检查状态。
 */
typedef enum {
  GST_STATE_VOID_PENDING        = 0,
  /* NULL状态或元素的初始状态。 */
  GST_STATE_NULL                = 1,
  /* 元素已经准备好进入暂停状态。 */
  GST_STATE_READY               = 2,
  /* 元素暂停时，它已准备好接受和处理数据。然而，Sink元素只接受一个缓冲区，然后阻塞。 */
  GST_STATE_PAUSED              = 3,
  /* 元素是PLAYING， #GstClock正在运行，数据正在流动。 */
  GST_STATE_PLAYING             = 4
} GstState;
```
#### 2.2.2 GstStateChangeReturn

```c
/* gstelement.h */

/**
 * 状态更改函数(如gst_element_set_state())的可能返回值。只有@GST_STATE_CHANGE_FAILURE才是真正的失败。
 */
typedef enum {
  GST_STATE_CHANGE_FAILURE             = 0,
  GST_STATE_CHANGE_SUCCESS             = 1,
  /* 状态更改将异步发生 */
  GST_STATE_CHANGE_ASYNC               = 2,
  /* 状态更改成功，但元素无法产生%GST_STATE_PAUSED中的数据。这通常发生在实时资源上。 */
  GST_STATE_CHANGE_NO_PREROLL          = 3
} GstStateChangeReturn;
```

#### 2.2.3 GstStateChangeReturn

```c
/* gstelement.h */

/**
 * GstStateChange:
 * @GST_STATE_CHANGE_NULL_TO_READY    : 从NULL状态到READY状态的状态改变。
 *   * 元素必须检查所需的资源是否可用。设备的sink和source通常会尝试探测设备以限制它们的caps。
 *   * 元素打开设备（以便一些特性可以被probe）。
 * @GST_STATE_CHANGE_READY_TO_PAUSED  : 从READY状态到PAUSED状态的状态改变。
 *   * 元素的pad被激活以准备接收数据。开始流线程。
 *   * 一些元素可能需要返回%GST_STATE_CHANGE_ASYNC，并在有足够信息时完成状态改变。对于sink来说，返回%GST_STATE_CHANGE_ASYNC
 *     当它们收到第一个缓冲区或%GST_EVENT_EOS（预滚动）时，完成状态改变。
 *     在PAUSED状态下，当收到第一个缓冲区或%GST_EVENT_EOS（预滚动）时，sink还会阻塞数据流。 
 *   * 管道将running_time重置为0。
 *   * 实时源返回%GST_STATE_CHANGE_NO_PREROLL并且不生成数据。
 * @GST_STATE_CHANGE_PAUSED_TO_PLAYING: 从PAUSED状态到PLAYING状态的状态改变。
 *   * 大多数元素忽略此状态改变。
 *   * 管道选择一个#GstClock并在将其设置为PLAYING之前将其分发给所有子元素。这意味着只允许在PLAYING状态下对#GstClock进行同步。
 *   * The pipeline selects a #GstClock and distributes this to all the children
 *     before setting them to PLAYING. This means that it is only allowed to
 *     synchronize on the #GstClock in the PLAYING state.
 *   * The pipeline uses the #GstClock and the running_time to calculate the
 *     base_time. The base_time is distributed to all children when performing
 *     the state change.
 *   * Sink elements stop blocking on the preroll buffer or event and start
 *     rendering the data.
 *   * Sinks can post %GST_MESSAGE_EOS in the PLAYING state. It is not allowed
 *     to post %GST_MESSAGE_EOS when not in the PLAYING state.
 *   * While streaming in PAUSED or PLAYING elements can create and remove
 *     sometimes pads.
 *   * Live sources start generating data and return %GST_STATE_CHANGE_SUCCESS.
 * @GST_STATE_CHANGE_PLAYING_TO_PAUSED: state change from PLAYING to PAUSED.
 *   * Most elements ignore this state change.
 *   * The pipeline calculates the running_time based on the last selected
 *     #GstClock and the base_time. It stores this information to continue
 *     playback when going back to the PLAYING state.
 *   * Sinks unblock any #GstClock wait calls.
 *   * When a sink does not have a pending buffer to play, it returns
 *     #GST_STATE_CHANGE_ASYNC from this state change and completes the state
 *     change when it receives a new buffer or an %GST_EVENT_EOS.
 *   * Any queued %GST_MESSAGE_EOS items are removed since they will be reposted
 *     when going back to the PLAYING state. The EOS messages are queued in
 *     #GstBin containers.
 *   * Live sources stop generating data and return %GST_STATE_CHANGE_NO_PREROLL.
 * @GST_STATE_CHANGE_PAUSED_TO_READY  : state change from PAUSED to READY.
 *   * Sinks unblock any waits in the preroll.
 *   * Elements unblock any waits on devices
 *   * Chain or get_range functions return %GST_FLOW_FLUSHING.
 *   * The element pads are deactivated so that streaming becomes impossible and
 *     all streaming threads are stopped.
 *   * The sink forgets all negotiated formats
 *   * Elements remove all sometimes pads
 * @GST_STATE_CHANGE_READY_TO_NULL    : state change from READY to NULL.
 *   * Elements close devices
 *   * Elements reset any internal state.
 * @GST_STATE_CHANGE_NULL_TO_NULL       : state change from NULL to NULL. (Since: 1.14)
 * @GST_STATE_CHANGE_READY_TO_READY     : state change from READY to READY,
 * This might happen when going to PAUSED asynchronously failed, in that case
 * elements should make sure they are in a proper, coherent READY state. (Since: 1.14)
 * @GST_STATE_CHANGE_PAUSED_TO_PAUSED   : state change from PAUSED to PAUSED.
 * This might happen when elements were in PLAYING state and 'lost state',
 * they should make sure to go back to real 'PAUSED' state (prerolling for example). (Since: 1.14)
 * @GST_STATE_CHANGE_PLAYING_TO_PLAYING : state change from PLAYING to PLAYING. (Since: 1.14)
 *
 * These are the different state changes an element goes through.
 * %GST_STATE_NULL &rArr; %GST_STATE_PLAYING is called an upwards state change
 * and %GST_STATE_PLAYING &rArr; %GST_STATE_NULL a downwards state change.
 */
typedef enum /*< flags=0 >*/
{
  GST_STATE_CHANGE_NULL_TO_READY        = (GST_STATE_NULL<<3) | GST_STATE_READY,
  GST_STATE_CHANGE_READY_TO_PAUSED      = (GST_STATE_READY<<3) | GST_STATE_PAUSED,
  GST_STATE_CHANGE_PAUSED_TO_PLAYING    = (GST_STATE_PAUSED<<3) | GST_STATE_PLAYING,

  GST_STATE_CHANGE_PLAYING_TO_PAUSED    = (GST_STATE_PLAYING<<3) | GST_STATE_PAUSED,
  GST_STATE_CHANGE_PAUSED_TO_READY      = (GST_STATE_PAUSED<<3) | GST_STATE_READY,
  GST_STATE_CHANGE_READY_TO_NULL        = (GST_STATE_READY<<3) | GST_STATE_NULL,

  GST_STATE_CHANGE_NULL_TO_NULL         = (GST_STATE_NULL<<3) | GST_STATE_NULL,
  GST_STATE_CHANGE_READY_TO_READY       = (GST_STATE_READY<<3) | GST_STATE_READY,
  GST_STATE_CHANGE_PAUSED_TO_PAUSED     = (GST_STATE_PAUSED<<3) | GST_STATE_PAUSED,
  GST_STATE_CHANGE_PLAYING_TO_PLAYING   = (GST_STATE_PLAYING<<3) | GST_STATE_PLAYING
} GstStateChange;
```

#### 2.2.4 GstStateChangeReturn

```c
/* gstelement.h */

```

### 2.3 GstElement相关结构体

## 3 GstElement对象相关函数