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

gst_mini_object_ref 和 gst_mini_object_unref 分别用于增加和减少引用计数。mini-object 的引用计数达到 0 时，首先调用 dispose 函数，如果此函数返回 TRUE，则调用mini-object的 free 函数。（具体可参考`gst_mini_object_unref`函数定义）

可以使用 gst_mini_object_copy 来复制mini-object。

当对象的引用计数恰好为 1 且没有父对象或只有一个父对象且该父对象本身是可写的时，gst_mini_object_is_writable 将返回 TRUE，这意味着当前调用者拥有该对象的唯一引用。gst_mini_object_make_writable 将返回对象的可写版本，如果引用计数不为 1，则可能是一个新的副本。

可以使用 gst_mini_object_set_qdata 和 gst_mini_object_get_qdata 将不透明数据与 GstMiniObject 关联。这些数据意在特定于特定对象，并且不会自动随着 gst_mini_object_copy 或类似方法一起复制。

可以分别使用 gst_mini_object_weak_ref 和 gst_mini_object_weak_unref 来添加和移除弱引用。

### 1.1 继承于GstMiniObject类的轻量级对象

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


## 2 GstMiniObject类型结构

`GstMiniObject` 是一个GBoxed类型，类型注册函数由 `gst_init` 调用。

**NOTE**：

GStreamer中所有的GBoxed类型对象，得到类型函数和类型值是分离的。并不是：

```c
#define GST_TYPE_MINI_OBJECT gst_mini_object_get_type()
```

而是：
```c
/* filename:gstminiobject.h */
extern unsigned long _gst_mini_object_type;
#define GST_TYPE_MINI_OBJECT (_gst_mini_object_type)

/* filename:gstminiobject.c */
unsigned long _gst_mini_object_type = 0;
_gst_mini_object_type = gst_mini_object_get_type ();
```
### 2.1 GstMiniObject类型注册宏定义

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

### 2.2 GstMiniObject类型相关枚举定义

#### 2.2.1 GstMiniObjectFlags

```c
/* filename: gstminiobject.h  */
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

```

该枚举类型是初始化函数中使用，标记该对象是只能还是读写模式。

```c
void
gst_mini_object_init (GstMiniObject * mini_object, guint flags, GType type,
    GstMiniObjectCopyFunction copy_func,
    GstMiniObjectDisposeFunction dispose_func,
    GstMiniObjectFreeFunction free_func) {

  ···
  mini_object->flags = flags;
  ···
}
```

#### 2.2.2 GstLockFlags

```c
/* filename: gstminiobject.h  */
/**
 * GstLockFlags:
 * @GST_LOCK_FLAG_READ: 用于读取访问的锁定
 * @GST_LOCK_FLAG_WRITE: 用于写入访问的锁定
 * @GST_LOCK_FLAG_EXCLUSIVE: 用于独占访问的锁定
 * @GST_LOCK_FLAG_LAST: 一般用来占位，Mask等用途
 *
 */
typedef enum {
  GST_LOCK_FLAG_READ      = (1 << 0),
  GST_LOCK_FLAG_WRITE     = (1 << 1),
  GST_LOCK_FLAG_EXCLUSIVE = (1 << 2),

  GST_LOCK_FLAG_LAST      = (1 << 8)
} GstLockFlags;
```

该枚举类型在 `gst_mini_object_lock` 和 `gst_mini_object_unlock` 函数中使用。结构体`_GstMiniObject` 的 `gint lockstate`成员有关。

#### 2.2.3 priv_pointer锁枚举类型

```c

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
该枚举类型在 `lock_priv_pointer` 函数中使用。用于锁定`priv_pointer`，结构体`_GstMiniObject` 的 `guint priv_uint`成员有关。

### 2.3 GstMiniObject类型相关结构体定义

`GstMiniObject`是主要结构体，成员`priv_pointer`有时候存储的就是结构体`PrivData`。

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

/* filename: gstminiobject.c */
typedef struct {
  GQuark quark;
  GstMiniObjectNotify notify;
  gpointer data;
  GDestroyNotify destroy;
} GstQData;

typedef struct {
  /* Atomic spinlock: 1 if locked, 0 otherwise */
  gint parent_lock;
  guint n_parents, n_parents_len;
  GstMiniObject **parents;

  guint n_qdata, n_qdata_len;
  GstQData *qdata;
} PrivData;
```

## 3 GstMiniObject对象函数总结

### 3.1 初始化函数

该初始化函数仅仅就是把输入参数赋值给GstMiniObject结构体

```c
/**
 * gst_mini_object_init: (skip)
 * @brief: 用所需要的 copy、dispose、free 函数初始化一个GstMiniObject对象
 * @mini_object: a #GstMiniObject
 * @flags: initial #GstMiniObjectFlags
 * @type: the #GType of the mini-object to create
 * @copy_func: (allow-none): the copy function, or %NULL
 * @dispose_func: (allow-none): the dispose function, or %NULL
 * @free_func: (allow-none): the free function or %NULL
 */
void
gst_mini_object_init (GstMiniObject * mini_object, guint flags, GType type,
    GstMiniObjectCopyFunction copy_func,
    GstMiniObjectDisposeFunction dispose_func,
    GstMiniObjectFreeFunction free_func)
{
  mini_object->type = type;
  mini_object->refcount = 1;
  mini_object->lockstate = 0;
  mini_object->flags = flags;

  mini_object->copy = copy_func;
  mini_object->dispose = dispose_func;
  mini_object->free = free_func;

  /* 默认设定为没有父对象状态 */
  g_atomic_int_set ((gint *) & mini_object->priv_uint,
      PRIV_DATA_STATE_NO_PARENT);
  mini_object->priv_pointer = NULL;

  GST_TRACER_MINI_OBJECT_CREATED (mini_object);
}
```

### 3.2 GstMiniObject对象锁函数

根据GstLockFlags枚举类型，我们可以发现，一共有三种Flag，读、写和独有锁。

操作的GstMiniObject成员的`lockstate`变量，该变量是gint类型，一共有4个字节。

- 第一个字节：存储读或者写状态，也就是0x01或者0x02。

- 第二个字节：比如说，有三个用户进行读，第二个字节就是存储有多少个用户进行读或者写。0x301表示三个用户就行读。

- 第三个字节：独有锁，有几个用户上独有锁，这个变量存储在第三个字节。0x20000。

根据下面代码发现，如果处于写锁状态，还可以再上成功写锁状态。最好写锁和独有锁一起上。可以避免同时多个写锁上成功。

#### 3.2.1 gst_mini_object_lock

循环处理，最后 `g_atomic_int_compare_and_exchange` 是巧妙的多线程方式。

```c
/**
 * @brief: 用指定的GstLockFlags锁定GstMiniObject对象的访问状态
 * @note: lockstate是一个gint变量，该变量具有4个字节
 *        第一个字节：标记读写状态 0x01是读，0x02是
 *        第二个字节是：上了几个锁 LOCK_ONE （有几个对象上了锁） 0x100
 *        第三个字节是：GST_LOCK_FLAG_EXCLUSIVE有几个 0x10000
 * @return: 如果被锁成功，返回TRUE
*/
gboolean
gst_mini_object_lock (GstMiniObject * object, GstLockFlags flags)
{
  guint access_mode, state, newstate;

  /* 检测object指针是否为NULL */
  g_return_val_if_fail (object != NULL, FALSE);
  /* 检查object对象可否能够进行上锁 */
  g_return_val_if_fail (GST_MINI_OBJECT_IS_LOCKABLE (object), FALSE);

  /* 如果该对象初始化flag就是只读，现在又要进行写锁，则返回FALSE */
  if (G_UNLIKELY (object->flags & GST_MINI_OBJECT_FLAG_LOCK_READONLY &&
          flags & GST_LOCK_FLAG_WRITE))
    return FALSE;

  do {
    /* 获取进行锁的模式, FLAG_MASK就是0xFF */
    access_mode = flags & FLAG_MASK;
    /* 获取该对象目前锁的状态，初始化的时候 lockstate = 0;  */
    newstate = state = (guint) g_atomic_int_get (&object->lockstate);

    GST_CAT_TRACE (GST_CAT_LOCKING, "lock %p: state %08x, access_mode %u",
        object, state, access_mode);

    /* 如果传入参数独有锁，则执行 */
    if (access_mode & GST_LOCK_FLAG_EXCLUSIVE) {
      /* shared ref，读写独有锁占用的是Flag的前八位，共享引用计数占用的是16位以后 */  
      newstate += SHARE_ONE; /* newstate = newstate + 0x10000*/
      access_mode &= ~GST_LOCK_FLAG_EXCLUSIVE; /* access_mode 已经获取了独有锁flag，去掉独有锁，查看还剩下什么flag */
    }

    /**
     * 如果对象已经处于写锁状态或者请求写锁，而且
     * shared 计数 >= 2 ，上锁失败
    */
    if (((state & GST_LOCK_FLAG_WRITE) != 0
            || (access_mode & GST_LOCK_FLAG_WRITE) != 0)
        && IS_SHARED (newstate))
      goto lock_failed;
    

    if (access_mode) {
      if ((state & LOCK_FLAG_MASK) == 0) { /* 该对象没有处于任何锁状态，也就是 state = 0 */
        /* nothing mapped, set access_mode */
        newstate |= access_mode; /* 把请求上的锁，赋值给 newstate */
      } else {
        /* access_mode must match */
        if ((state & access_mode) != access_mode) /* 具有写锁的时候不能上读锁，具有读锁的时候不能上写锁*/
          goto lock_failed;
      }
      /* increase refcount */
      newstate += LOCK_ONE;
    }
  } while (!g_atomic_int_compare_and_exchange (&object->lockstate, state,
          newstate));

  return TRUE;

lock_failed:
  {
    GST_CAT_DEBUG (GST_CAT_LOCKING,
        "lock failed %p: state %08x, access_mode %u", object, state,
        access_mode);
    return FALSE;
  }
}
```

#### 3.2.2 gst_mini_object_unlock

```c

/**
 * @brief: 用指定的GstLockFlags解除GstMiniObject对象的访问状态
*/
void
gst_mini_object_unlock (GstMiniObject * object, GstLockFlags flags)
{
  guint access_mode, state, newstate;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GST_MINI_OBJECT_IS_LOCKABLE (object));

  do {
    access_mode = flags & FLAG_MASK;
    newstate = state = (guint) g_atomic_int_get (&object->lockstate);

    GST_CAT_TRACE (GST_CAT_LOCKING, "unlock %p: state %08x, access_mode %u",
        object, state, access_mode);
    
    /* 先去除独有锁的标记 */
    if (access_mode & GST_LOCK_FLAG_EXCLUSIVE) {
      /* shared counter */
      g_return_if_fail (state >= SHARE_ONE);
      newstate -= SHARE_ONE;
      access_mode &= ~GST_LOCK_FLAG_EXCLUSIVE; /* 去除传入参数中的独有锁 */
    }

    if (access_mode) {
      g_return_if_fail ((state & access_mode) == access_mode);
      /* decrease the refcount */
      newstate -= LOCK_ONE;
      /* last refcount, unset access_mode */
      if ((newstate & LOCK_FLAG_MASK) == access_mode) /* 如果是最后一个锁引用，把第一第二个字节置零 */
        newstate &= ~LOCK_FLAG_MASK;
    }
  } while (!g_atomic_int_compare_and_exchange (&object->lockstate, state,
          newstate));
}
```

### 3.3 GstMiniObject是否可写

- 不可写状态：

  1.这是一个能够上锁的GstMiniObject对象
      如果被上两次独有锁，也就是处于两次共享状态，直接返回不可写

  2.这是一个只读的GstMiniObject对象，引用计数不等于1，直接返回不可写
 
- 可写状况： 对象可锁，只有一个用户上独有锁或者对象只读，引用计数等于 1 的时候

  1. 只有一个用户上独有锁，只有一个父对象且父对象本身是可写的，返回可写
  
  2. 只有一个用户上独有锁，没有父对象，返回可写。

#### 3.3.1 gst_mini_object_is_writable

```c
/**
 * gst_mini_object_is_writable:
 * @mini_object: 要检查的小型对象
 *
 * 如果 @mini_object 设置了 LOCKABLE 标志，检查当前对 @object 的独占锁定是否是唯一的，
 * 这意味着对对象的更改不会对其他任何对象可见。
 *
 * 如果没有设置 LOCKABLE 标志，检查 @mini_object 的引用计数是否正好为 1，
 * 这意味着没有其他对该对象的引用，因此该对象是可写的。
 *
 * 修改小型对象应该只在确认它是可写的之后进行。
 *
 * 返回：如果对象是可写的，则为 %TRUE。
 */

gboolean
gst_mini_object_is_writable (const GstMiniObject * mini_object)
{
  gboolean result;
  gint priv_state;

  g_return_val_if_fail (mini_object != NULL, FALSE);

  /* 检查 GstMiniObject 对象创建的时候，是否标记了能够锁的Flag */
  if (GST_MINI_OBJECT_IS_LOCKABLE (mini_object)) { /* 创建的时候是：GST_MINI_OBJECT_FLAG_LOCKABLE */
    /* 是否处于被共享状态 */
    result = !IS_SHARED (g_atomic_int_get (&mini_object->lockstate));
  } else { /* 创建的时候是其他Flag */
    result = (GST_MINI_OBJECT_REFCOUNT_VALUE (mini_object) == 1);
  }

  /**
   * SHARED只跟独有锁有关，如果该对象被两个用户独有，就不能被写。
  */
  if (!result) /* 如果处于共享状态，那就一定是可写的，直接返回 FALSE */
    return result;
  
  /* 如果不是 PRIV_DATA_STATE_PARENTS_OR_QDATA，堵塞等待可切换到 PRIV_DATA_STATE_LOCKED状态， 返回的 priv_state 是锁之前的状态*/
  priv_state = lock_priv_pointer (GST_MINI_OBJECT_CAST (mini_object));

    /* 现在我们要么需要检查完整的结构体以及其中的所有父对象，
     * 要么如果确实只有一个父对象，我们可以检查那一个 */
  if (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    PrivData *priv_data = mini_object->priv_pointer;

    /* Lock parents（ parent_lock 对应 1 是 锁， 0 不是锁 ） */
    while (!g_atomic_int_compare_and_exchange (&priv_data->parent_lock, 0, 1));
    
    /**
     * @note: 如果我们有一个父对象，我们只有在那个父对象是可写的情况下才是可写的。
     *        我们没有父对象，我们就是可写的
     *        否则，如果我们有多个父对象，我们就不是可写的
    */
    if (priv_data->n_parents == 1)
      result = gst_mini_object_is_writable (priv_data->parents[0]); /* 对应基本概念中的：只有一个父对象且该父对象本身是可写的时，对象可写*/
    else if (priv_data->n_parents == 0)
      result = TRUE;
    else
      result = FALSE;

    /* Unlock again（恢复到锁之前的状态） */
    g_atomic_int_set (&priv_data->parent_lock, 0);
  } else {
    if (priv_state == PRIV_DATA_STATE_ONE_PARENT) {
      result = gst_mini_object_is_writable (mini_object->priv_pointer);
    } else {
      g_assert (priv_state == PRIV_DATA_STATE_NO_PARENT);
      result = TRUE;
    }

    /* Unlock again （恢复到锁之前的状态） */
    g_atomic_int_set ((gint *) & mini_object->priv_uint, priv_state);
  }

  return result;
}
```

#### 3.3.2 gst_mini_object_make_writable

```c
/**
 * gst_mini_object_make_writable: (跳过)
 * @mini_object: (完全转移): 要使其可写的GstMiniObject对象
 *
 * 检查一个对象是否可写。如果不可写，将创建并返回一个可写的副本。
 * 这会放弃对原始GstMiniObject对象的引用，并返回对新对象的引用。
 *
 * 多线程安全
 *
 * 返回：(完全转移) (可空)：一个可写的GstMiniObject对象（可能与 @mini_object 相同，也可能不同）
 *     或者如果需要复制但不可能时返回 %NULL。
 */

GstMiniObject *
gst_mini_object_make_writable (GstMiniObject * mini_object)
{
  GstMiniObject *ret;

  g_return_val_if_fail (mini_object != NULL, NULL);

  if (gst_mini_object_is_writable (mini_object)) {
    ret = mini_object;
  } else {
    ret = gst_mini_object_copy (mini_object);
    GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "copy %s miniobject %p -> %p",
        g_type_name (GST_MINI_OBJECT_TYPE (mini_object)), mini_object, ret);
    gst_mini_object_unref (mini_object);
  }

  return ret;
}
```

### 父对象

### GstQData

### 虚引用






### gst_mini_object_copy

```c
GstMiniObject *
gst_mini_object_copy (const GstMiniObject * mini_object) {

  ...

  GstMiniObject *copy = mini_object->copy (mini_object);

  ...
}
```