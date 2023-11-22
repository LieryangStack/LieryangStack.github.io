---
layout: post
title: 二、GstMiniObject
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstMiniObject基本概念

"GstMiniObject" 是一个简单的结构体，可用于实现引用计数类型。

- GstMiniObject使用的是 `G_DEFINE_BOXED_TYPE` 进行的定义。（也就是结构体对象，并没有普通对象的信号，属性等功能），GstMiniObject内部实现了引用计数功能。

- 具体G_DEFINE_BOXED_TYPE可以参考GObject学习笔记

- 因为不是GObject对象，所以不能使用GObject的相关函数，比如常用的 g_object_new, g_object_unref等函数。仅仅是注册一个GType类型。

子类将在其结构体中首先包含 "GstMiniObject"，然后调用 gst_mini_object_init 来初始化 "GstMiniObject" 字段。

gst_mini_object_ref 和 gst_mini_object_unref 分别用于增加和减少引用计数。mini-object 的引用计数达到 0 时，首先调用 dispose 函数，如果此函数返回 TRUE，则调用mini-object的 free 函数。

可以使用 gst_mini_object_copy 来复制mini-object。

当对象的引用计数恰好为 1 且没有父对象或只有一个父对象且该父对象本身是可写的时，gst_mini_object_is_writable 将返回 TRUE，这意味着当前调用者拥有该对象的唯一引用。gst_mini_object_make_writable 将返回对象的可写版本，如果引用计数不为 1，则可能是一个新的副本。

可以使用 gst_mini_object_set_qdata 和 gst_mini_object_get_qdata 将不透明数据与 GstMiniObject 关联。这些数据意在特定于特定对象，并且不会自动随着 gst_mini_object_copy 或类似方法一起复制。

可以分别使用 gst_mini_object_weak_ref 和 gst_mini_object_weak_unref 来添加和移除弱引用。

## 2 继承于GstMiniObject类的轻量级对象

- **GstBuffer**: 表示流水线中单个媒体数据块。用于传输原始数据（如音频样本或视频帧）。

- **GstBufferList**: 管理 `GstBuffer` 对象的集合，适用于需要批量处理或优化传输的情况。

- **GstMessage**: 在 GStreamer 应用程序和元素之间传递异步消息，包括错误、状态变更、流结束通知等。

- **GstMemory**: 表示 `GstBuffer` 数据的内存块，抽象了数据可以存储在的不同类型的内存。

- **GstCaps**: 描述媒体数据的格式和属性，指定流水线元素可以处理或产生的媒体类型。

- **GstEvent**: 在流水线元素之间传递控制事件，如流开始、配置更改、跳到新的时间点等。

- **GstContext**: 在元素之间共享高层次信息，如设备句柄或平台特定数据。

- **GstSample**: 包含 `GstBuffer`、`GstCaps` 和时间戳，通常用于表示处理后的单个数据样本。

- **GstQuery**: 查询流水线或其元素的状态，如持续时间、位置、格式、带宽等。

- **GstDateTime**: 表示和处理日期和时间数据，用于处理时间相关的元数据或时间戳。


## 3 GstMiniObject 结构体成员

```c
/* filename: gstminiobject.h */
struct _GstMiniObject {
  GType   type; /* 对象注册的GType类型 */

  /*< public >*/ /* with COW */
  gint    refcount; /* 引用计数 */
  gint    lockstate; /* 该对象锁状态，GstLockFlags */
  guint   flags; /* 该对象flag，GstMiniObjectFlags */

  GstMiniObjectCopyFunction copy;
  GstMiniObjectDisposeFunction dispose;
  GstMiniObjectFreeFunction free;

  /* < private > */
  /* Used to keep track of parents, weak ref notifies and qdata */
  guint priv_uint; /* priv状态 这和私有Data状态有关，具体flag如 PRIV_DATA_STATE_LOCKED*/
  gpointer priv_pointer;  /* 指向 */
};


/**
 * GstMiniObjectFlags:
 * @GST_MINI_OBJECT_FLAG_LOCKABLE: 对象可以通过 gst_mini_object_lock() 和
 * gst_mini_object_unlock() 进行锁定和解锁。
 * @GST_MINI_OBJECT_FLAG_LOCK_READONLY: 对象被永久性地锁定在只读模式下。
 * 只能对该对象执行读取锁定。
 * @GST_MINI_OBJECT_FLAG_MAY_BE_LEAKED: 预期该对象即使在调用 gst_deinit() 之后
 * 也会保持存活，因此应该被内存泄漏检测工具忽略。（自版本 1.10 起可用）
 * @GST_MINI_OBJECT_FLAG_LAST: 子类可以使用的第一个标志。(可以被用来计算那个flag标记了)
 *
 */
typedef enum
{
  GST_MINI_OBJECT_FLAG_LOCKABLE      = (1 << 0),
  GST_MINI_OBJECT_FLAG_LOCK_READONLY = (1 << 1),
  GST_MINI_OBJECT_FLAG_MAY_BE_LEAKED = (1 << 2),
  /* padding */
  GST_MINI_OBJECT_FLAG_LAST          = (1 << 4)
} GstMiniObjectFlags;


/**
 * GstLockFlags:
 * @GST_LOCK_FLAG_READ: 用于读取访问的锁定
 * @GST_LOCK_FLAG_WRITE: 用于写入访问的锁定
 * @GST_LOCK_FLAG_EXCLUSIVE: 用于独占访问的锁定
 * @GST_LOCK_FLAG_LAST: 可用于自定义目的的第一个标志
 *
 */
typedef enum {
  GST_LOCK_FLAG_READ      = (1 << 0),
  GST_LOCK_FLAG_WRITE     = (1 << 1),
  GST_LOCK_FLAG_EXCLUSIVE = (1 << 2),

  GST_LOCK_FLAG_LAST      = (1 << 8)
} GstLockFlags;


/* filename: gstminiobject.c */

/**
 * GST_TYPE_MINI_OBJECT:
 *
 * 与 #GstMiniObject 相关联的 #GType。
 *
 * 自版本 1.20 起可用。
 */

/* 出于向后兼容的原因，我们在 GstMiniObject 结构中
 * 使用 guint 和 gpointer，以一种相当复杂的方式来存储父对象和 qdata。
 * 最初，它们仅仅是 qdata 的数量和 qdata 本身。
 *
 * guint 被用作一个原子状态整数，具有以下状态：
 * - Locked：0，基本上是一个自旋锁
 * - No parent，无 qdata：1（指针为 NULL）
 * - 一个父对象：2（指针包含父对象）
 * - 多个父对象或 qdata：3（指针包含一个 PrivData 结构体）
 *
 * 除非我们处于状态 3，否则我们总是必须原子性地移动到锁定状态，
 * 并在稍后再将其释放回目标状态，以便在访问指针时使用。
 * 当我们处于状态 3 时，我们将不再转移到更低的状态
 *
 * FIXME 2.0：我们应该直接在结构体内部存储这些信息，
 * 可能直接为几个父对象保留空间
 */

/* 私有数据的三种状态 */
enum {
  PRIV_DATA_STATE_LOCKED = 0,
  PRIV_DATA_STATE_NO_PARENT = 1,
  PRIV_DATA_STATE_ONE_PARENT = 2,
  PRIV_DATA_STATE_PARENTS_OR_QDATA = 3,
};
```

## 4 GstMiniObject 类型的定义与注册

```c
/* filename:gstminiobject.h */

/* _gst_mini_object_type 是在类型系统中注册的类型值，该值由源文件定义，gst_init进行注册 */
extern unsigned long _gst_mini_object_type;
#define GST_TYPE_MINI_OBJECT (_gst_mini_object_type)

#define GST_DEFINE_MINI_OBJECT_TYPE(TypeName,type_name) \
   G_DEFINE_BOXED_TYPE(TypeName,type_name,              \
       (GBoxedCopyFunc) gst_mini_object_ref,            \
       (GBoxedFreeFunc) gst_mini_object_unref)

/* 注册该类型调用的函数 */
extern           gst_mini_object_get_type   (void);

/* filename: gstminiobject.c */

/* GstMiniObject类型值定义 */
unsigned long _gst_mini_object_type = 0;

/* gst_mini_object_get_type函数定义 
 * G_DEFINE_BOXED_TYPE 拷贝和释放使用的ref和unref函数指针
 */
GST_DEFINE_MINI_OBJECT_TYPE (GstMiniObject, gst_mini_object);

/* 注册GstMiniObject类型，该函数由gst_init调用 */
void
_priv_gst_mini_object_initialize (void)
{
  _gst_mini_object_type = gst_mini_object_get_type ();
  weak_ref_quark = g_quark_from_static_string ("GstMiniObjectWeakRefQuark");
}

```

## gstminiobject.c 定义函数分析

### gst_mini_object_init

该初始化函数仅仅就是把输入参数赋值给GstMiniObject结构体

```c
void
gst_mini_object_init (GstMiniObject * mini_object, guint flags, GType type,
    GstMiniObjectCopyFunction copy_func,
    GstMiniObjectDisposeFunction dispose_func,
    GstMiniObjectFreeFunction free_func)
```

### gst_mini_object_copy

```c
GstMiniObject *
gst_mini_object_copy (const GstMiniObject * mini_object) {

  ...

  GstMiniObject *copy = mini_object->copy (mini_object);

  ...
}
```