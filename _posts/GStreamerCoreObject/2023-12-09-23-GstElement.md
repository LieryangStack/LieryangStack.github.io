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
 *   1. 元素必须检查所需的资源是否可用。设备的sink和source通常会尝试探测设备以限制它们的caps。
 *   2. 元素打开设备（以便一些特性可以被probe）。
 * @GST_STATE_CHANGE_READY_TO_PAUSED  : 从READY状态到PAUSED状态的状态改变。
 *   1. 元素的pad被激活以准备接收数据。开始流线程。
 *   2. 一些元素可能需要返回%GST_STATE_CHANGE_ASYNC，并在有足够信息时完成状态改变。对于sink来说，返回%GST_STATE_CHANGE_ASYNC
 *     当它们收到第一个缓冲区或%GST_EVENT_EOS（预滚动）时，完成状态改变。
 *     在PAUSED状态下，当收到第一个缓冲区或%GST_EVENT_EOS（预滚动）时，sink还会阻塞数据流。 
 *   3. 管道将running_time重置为0。
 *   4. 实时源返回%GST_STATE_CHANGE_NO_PREROLL并且不生成数据。
 * @GST_STATE_CHANGE_PAUSED_TO_PLAYING: 从PAUSED状态到PLAYING状态的状态改变。
 *   1. 大多数元素忽略此状态改变。
 *   2. 管道选择一个#GstClock并在将其设置为PLAYING之前将其分发给所有子元素。这意味着只允许在PLAYING状态下对#GstClock进行同步。
 *   3. 管道使用 #GstClock和 running_time来计算base_time。在执行状态改变时，将base_time分发给所有子元素。
 *   4. Sink元素停止在预滚动prepoll缓冲区或事件上阻塞，并开始渲染数据。
 *   5. The pipeline uses the #GstClock and the running_time to calculate the
 *   6. Sink可以在PLAYING状态下发布%GST_MESSAGE_EOS。不允许在不是PLAYING状态时发布%GST_MESSAGE_EOS。
 *   7. 在PAUSED或PLAYING状态中进行流式传输时，元素有时会创建和移除pad。
 *   8. 实时源开始生成数据并返回%GST_STATE_CHANGE_SUCCESS。
 * 
 * @GST_STATE_CHANGE_PLAYING_TO_PAUSED: 从 PLAYING 到 PAUSED 的状态变化。
 *   1. Sink 解除所有 #GstClock 的等待调用。
 *   2. 当 Sink 没有待播放的缓冲区时，它会从此状态变化返回 #GST_STATE_CHANGE_ASYNC，并在接收到新的缓冲区或 %GST_EVENT_EOS 时完成状态变化。
 *   3. 任何排队的 %GST_MESSAGE_EOS 都会被移除，因为它们将在返回 PLAYING 状态时重新发布。这些 EOS 消息被排在 #GstBin 容器中。
 *   4. 实时源停止生成数据并返回 %GST_STATE_CHANGE_NO_PREROLL。
 * @GST_STATE_CHANGE_PAUSED_TO_READY  : 从 PAUSED 到 READY 的状态变化。
 *   1. Sink 解除 preroll 中的所有等待。
 *   2. 元素解除设备上的所有等待。
 *   3. Chain 或 get_range 函数返回 %GST_FLOW_FLUSHING。
 *   4. 元素的 pads 被停用，因此无法进行流传输，并停止所有流传输线程。
 *   5. Sink 忘记所有已协商的格式。
 *   6. 元素移除所有 sometimes pads。
 * @GST_STATE_CHANGE_READY_TO_NULL    : 从 READY 到 NULL 的状态变化。
 *   1. 元素关闭设备。
 *   2. 元素重置任何内部状态。
 * 
 * @GST_STATE_CHANGE_NULL_TO_NULL       : state change from NULL to NULL. (Since: 1.14)
 * @GST_STATE_CHANGE_READY_TO_READY     : state change from READY to READY,
 * This might happen when going to PAUSED asynchronously failed, in that case
 * 这可能发生在异步转换到 PAUSED 失败的情况下，在这种情况下，元素应确保它们处于适当、一致的 READY 状态。 (自 1.14 版本开始)
 * @GST_STATE_CHANGE_PAUSED_TO_PAUSED   : state change from PAUSED to PAUSED.
 * 这可能发生在元素处于 PLAYING 状态但是 'lost state' 的情况下，它们应确保返回真正的 'PAUSED' 状态（例如，预滚动）。 (自 1.14 版本开始)
 * @GST_STATE_CHANGE_PLAYING_TO_PLAYING : state change from PLAYING to PLAYING. (Since: 1.14)
 *
 * 这些是元素经历的不同状态变化。
 * %GST_STATE_NULL ⇒ %GST_STATE_PLAYING 被称为上升状态变化，
 * 而 %GST_STATE_PLAYING ⇒ %GST_STATE_NULL 则是下降状态变化。
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

/**
 * GstElementFlags:
 * @GST_ELEMENT_FLAG_LOCKED_STATE: ignore state changes from parent
 * @GST_ELEMENT_FLAG_SINK: the element is a sink
 * @GST_ELEMENT_FLAG_SOURCE: the element is a source.
 * @GST_ELEMENT_FLAG_PROVIDE_CLOCK: the element can provide a clock
 * @GST_ELEMENT_FLAG_REQUIRE_CLOCK: the element requires a clock
 * @GST_ELEMENT_FLAG_INDEXABLE: the element can use an index
 * @GST_ELEMENT_FLAG_LAST: offset to define more flags
 *
 * 元素可能具有的标准flags。
 */
typedef enum
{
  GST_ELEMENT_FLAG_LOCKED_STATE   = (GST_OBJECT_FLAG_LAST << 0),
  GST_ELEMENT_FLAG_SINK           = (GST_OBJECT_FLAG_LAST << 1),
  GST_ELEMENT_FLAG_SOURCE         = (GST_OBJECT_FLAG_LAST << 2),
  GST_ELEMENT_FLAG_PROVIDE_CLOCK  = (GST_OBJECT_FLAG_LAST << 3),
  GST_ELEMENT_FLAG_REQUIRE_CLOCK  = (GST_OBJECT_FLAG_LAST << 4),
  GST_ELEMENT_FLAG_INDEXABLE      = (GST_OBJECT_FLAG_LAST << 5),
  /* padding */
  GST_ELEMENT_FLAG_LAST           = (GST_OBJECT_FLAG_LAST << 10)
} GstElementFlags;
```

### 2.3 GstElement相关结构体

#### 2.3.1 GstElement

```c
/**
 * GstElement:
 * @state_lock: 多线程执行  gst_element_set_state() 的锁
 * @state_cond: 通知其它条件锁等待，状态改变完成
 * @state_cookie: 用于检测 gst_element_set_state() 和 gst_element_get_state() 的并发执行
 * @target_state: 应用程序设置的元素的目标状态
 * @current_state: 元素当前的状态
 * @next_state: 元素的下一个状态，如果元素正处于正确的状态，则可以是 #GST_STATE_VOID_PENDING
 * @pending_state: 元素应该转换到的最终状态，如果元素处于正确的状态，则可以是 #GST_STATE_VOID_PENDING
 * @last_return: 元素状态改变的最后返回值
 * @bus: 元素的bus总线，这个总线由父元素或应用程序提供给元素。#GstPipeline 有自己的总线。
 * @clock: 元素的时钟。通常由顶层 #GstPipeline 提供给元素。
 * @base_time: 元素设置为 PLAYING 之前时钟的时间。在 PLAYING 状态下，从当前时钟时间减去 @base_time 将得到相对于时钟的 running_time。
 * @start_time: 上一个 PAUSED 状态的 running_time
 * @numpads: pads的数量，包括source pads和sink pads.
 * @pads: (element-type Gst.Pad): 存储pads的列表
 * @numsrcpads: source pads的数量
 * @srcpads: (element-type Gst.Pad): source pads的数列表
 * @numsinkpads: sink pads的数量
 * @sinkpads: (element-type Gst.Pad): sink pads的列表
 * @pads_cookie: 每当添加或移除一个 pad 时更新
 * @contexts: (element-type Gst.Context): 上下文列表
 *
 * GStreamer element abstract base class.
 */
struct _GstElement
{
  GstObject             object; /* 用于 */

  /*< public >*/ /* with LOCK */
  GRecMutex             state_lock;

  /* element state */
  GCond                 state_cond;
  guint32               state_cookie; /* 每次设定元素到新的状态，这个值就会加一（增量值，不会减少） */
  GstState              target_state; /* 应用程序设定的元素目标状态，元素目标状态不应该和pengding_state相同吗？？ */
  GstState              current_state; /* 元素当前状态 */
  GstState              next_state; /* 元素的下一个状态，如果元素正处于正确的状态，则可以是 #GST_STATE_VOID_PENDING */
  GstState              pending_state; /* 元素应该转换到的最终状态，如果元素处于正确的状态，则可以是 #GST_STATE_VOID_PENDING */
  GstStateChangeReturn  last_return; /* 上一次状态改变后的返回值 */

  GstBus               *bus;

  /* allocated clock */
  GstClock             *clock;
  GstClockTimeDiff      base_time; /* NULL/READY: 0 - PAUSED: current time - PLAYING: difference to clock */
  GstClockTime          start_time;

  /* element pads, these lists can only be iterated while holding
   * the LOCK or checking the cookie after each LOCK. */
  guint16               numpads;
  GList                *pads;
  guint16               numsrcpads;
  GList                *srcpads;
  guint16               numsinkpads;
  GList                *sinkpads;
  guint32               pads_cookie;

  /* with object LOCK */
  GList                *contexts;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING-1];
};
```

### 2.3.2 GstElementClass

```c
/**
 * GstElementClass:
 * @parent_class: the parent class structure
 * @metadata: 此类元素的元数据
 * @elementfactory: 创建这些元素的 #GstElementFactory
 * @padtemplates:  #GstPadTemplate 的 #GList
 * @numpadtemplates: padtemplates 的数量
 * @pad_templ_cookie:  每当 padtemplates 更改时都会更改
 * @request_new_pad: 请求新 pad 时调用
 * @release_pad: 当请求 pad 要释放时调用
 * @get_state: 获取元素的状态
 * @set_state: 在元素上设置新状态
 * @change_state: 被 @set_state 调用以执行增量状态更改
 * @set_bus: 在元素上设置 #GstBus
 * @provide_clock: 获取元素提供的 #GstClock
 * @set_clock: 在元素上设置 #GstClock
 * @send_event: 向元素发送 #GstEvent
 * @query: 在元素上执行 #GstQuery
 * @state_changed: 在设置新状态后立即调用。
 * @post_message: 在元素上发布消息时调用。链接到父类的处理程序以将其发布到总线上。
 * @set_context: 在元素上设置 #GstContext
 *
 * 重写以上虚函数以实现用户定义元素的功能
 */
struct _GstElementClass
{
  GstObjectClass         parent_class;

  /*< public >*/
  /* the element metadata */
  gpointer		 metadata;

  /* factory that the element was created from */
  GstElementFactory     *elementfactory;

  /* templates for our pads */
  GList                 *padtemplates;
  gint                   numpadtemplates;
  guint32                pad_templ_cookie;

  /*< private >*/
  /* signal callbacks */
  void (*pad_added)     (GstElement *element, GstPad *pad);
  void (*pad_removed)   (GstElement *element, GstPad *pad);
  void (*no_more_pads)  (GstElement *element);

  /*< public >*/
  /* virtual methods for subclasses */

  /* request/release pads */
  /* FIXME 2.0 harmonize naming with gst_element_request_pad */
  GstPad*               (*request_new_pad)      (GstElement *element, GstPadTemplate *templ,
                                                 const gchar* name, const GstCaps *caps);

  void                  (*release_pad)          (GstElement *element, GstPad *pad);

  /* state changes */
  GstStateChangeReturn (*get_state)             (GstElement * element, GstState * state,
                                                 GstState * pending, GstClockTime timeout);
  GstStateChangeReturn (*set_state)             (GstElement *element, GstState state);
  GstStateChangeReturn (*change_state)          (GstElement *element, GstStateChange transition);
  void                 (*state_changed)         (GstElement *element, GstState oldstate,
                                                 GstState newstate, GstState pending);

  /* bus */
  void                  (*set_bus)              (GstElement * element, GstBus * bus);

  /* set/get clocks */
  GstClock*             (*provide_clock)        (GstElement *element);
  gboolean              (*set_clock)            (GstElement *element, GstClock *clock);

  /* query functions */
  gboolean              (*send_event)           (GstElement *element, GstEvent *event);

  gboolean              (*query)                (GstElement *element, GstQuery *query);

  gboolean              (*post_message)         (GstElement *element, GstMessage *message);

  void                  (*set_context)          (GstElement *element, GstContext *context);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING_LARGE-2];
};
```

## 3 GstElement对象相关函数