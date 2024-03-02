---
layout: post
title: 八、GstBufferPool
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstBufferPool基本概念

- GstBufferPool 是一个用于re-allocate（预分配p）和recycle（回收再利用）具有相同大小和属性的buffer的对象。

- 通过 gst_buffer_pool_new 创建 GstBufferPool。创建后，需要进行配置。使用 gst_buffer_pool_get_config 获取池的当前配置结构。通过 gst_buffer_pool_config_set_params 和 gst_buffer_pool_config_set_allocator 可以配置bufferpool的参数和分配器。根据bufferpool的实现，还可以配置其他属性。

- 缓冲池bufferpool可以有额外的选项，可以通过 gst_buffer_pool_config_add_option 启用。可用选项可以通过 gst_buffer_pool_get_options 检索。某些选项允许设置额外的配置属性。

- 配置结构配置完成后，gst_buffer_pool_set_config 会在池中更新配置。如果配置结构不被接受，这可能会失败。配置完成后，可以使用 gst_buffer_pool_set_active 激活池。这将预分配池中配置的资源。

- 当池处于活动状态时，可以使用 gst_buffer_pool_acquire_buffer 从池中检索缓冲区。

- 从缓冲池分配的缓冲区在其引用计数降至 0 时，将通过 gst_buffer_pool_release_buffer 自动返回到池中。

- 缓冲池可以再次通过 gst_buffer_pool_set_active 停用。所有进一步的 gst_buffer_pool_acquire_buffer 调用将返回错误。当所有缓冲区被返回到池中时，它们将被释放。

## 1.1 GstBufferPool继承关系

```c
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstBufferPool
```

## 2 GstBufferPool类型结构

### 2.1 GstBufferPool类型注册宏定义

GstBufferPool 是继承 `GstObject` 的标准对象结构体。

```c
/* filename: gstbufferpool.c */
G_DEFINE_TYPE_WITH_PRIVATE (GstBufferPool, gst_buffer_pool, GST_TYPE_OBJECT);
```

### 2.2 GstBufferPool类型相关枚举

#### 2.2.1 GstBufferPoolAcquireFlags

`GstBufferPoolAcquireFlags` flags是用来控制buffer的分配。

```c
/* filename: gstbufferpool.h */
/**
 * GstBufferPoolAcquireFlags:
 * @GST_BUFFER_POOL_ACQUIRE_FLAG_NONE: no flags
 * @GST_BUFFER_POOL_ACQUIRE_FLAG_KEY_UNIT: buffer是关键帧
 * @GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT: 当bufferpool是空的时候，获取buffer默认是会被堵塞的，直到bufferpool有可获取的buffer。
 *                                         如果设定此flag，bufferpool是空的时候，默认返回 #GST_FLOW_EOS，而不是阻塞。
 * @GST_BUFFER_POOL_ACQUIRE_FLAG_DISCONT: buffer is discont
 * @GST_BUFFER_POOL_ACQUIRE_FLAG_LAST: last flag, subclasses can use private flags
 *    starting from this value.
 *
 * Additional flags to control the allocation of a buffer
 */
typedef enum {
  GST_BUFFER_POOL_ACQUIRE_FLAG_NONE     = 0,
  GST_BUFFER_POOL_ACQUIRE_FLAG_KEY_UNIT = (1 << 0),
  GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT = (1 << 1),
  GST_BUFFER_POOL_ACQUIRE_FLAG_DISCONT  = (1 << 2),
  GST_BUFFER_POOL_ACQUIRE_FLAG_LAST     = (1 << 16),
} GstBufferPoolAcquireFlags;
```

### 2.3 GstBufferPool相关结构体

#### 2.3.1 GstBufferPoolAcquireParams

```c
/* filename: gstbufferpool.h */
/**
 * GstBufferPoolAcquireParams:
 * @format: the format of @start and @stop
 * @start: the start position
 * @stop: the stop position
 * @flags: additional flags
 * 
 * Parameters传递给gst_buffer_pool_acquire_buffer()函数取控制buffer分配。
 * 
 * 默认的实现会忽略 @start 和 @stop 成员，但是其他的实现能够使用这些额外的信息去决定如何获取这些buffer
 */
struct _GstBufferPoolAcquireParams {
  GstFormat                 format;
  gint64                    start;
  gint64                    stop;
  GstBufferPoolAcquireFlags flags;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.2 GstBufferPool

```c
/* filename: gstbufferpool.h */
struct _GstBufferPool {
  GstObject            object;

  /*< protected >*/
  gint                 flushing; /* bufferpool当前是否正在回收未归还的buffer */

  /*< private >*/
  GstBufferPoolPrivate *priv;

  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.3 GstBufferPoolClass

```c
/* filename: gstbufferpool.h */
struct _GstBufferPoolClass {
  GstObjectClass    object_class;

  /*< public >*/

  /* 获取此bufferpool支持的选项列表 */
  const gchar ** (*get_options)    (GstBufferPool *pool);

  /* 应用缓冲池配置 */
  gboolean       (*set_config)     (GstBufferPool *pool, GstStructure *config);

  /* 启动缓冲池。默认实现将进行预分配min-buffers缓冲区并将它们放入队列。 */
  gboolean       (*start)          (GstBufferPool *pool);

  /* 停止缓冲池。默认实现将释放预分配缓冲区。当所有buffer都返回到bufferpool，这个函数被调用。 */
  gboolean       (*stop)           (GstBufferPool *pool);

  /**
   * @brief: 从@pool，获取到一个新的buffer，默认的实现将会从queue拿取一个buffer或者，（可选，通过flag）或者
   *         当没有buffer可获取的时候，阻塞等待有buffer可以获取。
   * @return: #GstFlowReturn，例如：当pool是没有激活状态，返回GST_FLOW_FLUSHING
   */
  GstFlowReturn  (*acquire_buffer) (GstBufferPool *pool, GstBuffer **buffer,
                                    GstBufferPoolAcquireParams *params);

  /**
   * @brief: 分配一个buffer。默认实现分配buffers是来自被配置的内存分配器和被配置的分配器参数变量。
   *         所有在被分配的buffer中的元数据就会被标记GST_META_FLAG_POOLED、GST_META_FLAG_LOCKED
   *         调用 GstBufferPoolClass::reset_buffer 不会从buffer中移除这些元数据
   * @return: #GstFlowReturn表示分配是否成功
   */
  GstFlowReturn  (*alloc_buffer)   (GstBufferPool *pool, GstBuffer **buffer,
                                    GstBufferPoolAcquireParams *params);

  /**
   * @brief: 将buffer重置到刚分配时候的状态。默认的实现方式将会清除flags、时间戳timestamps，
   *         并将删除没有#GST_META_FLAG_POOLED标志的元数据(即使是带有#GST_META_FLAG_LOCKED的元数据)。
   *         如果设置了#GST_BUFFER_FLAG_TAG_MEMORY，这个函数也可以尝试恢复内存并再次清除#GST_BUFFER_FLAG_TAG_MEMORY。
   */
  void           (*reset_buffer)   (GstBufferPool *pool, GstBuffer *buffer);

  /**
   * @brief: 释放@buffer回到bufferpool。默认的实现方式将把@buffer放回到queue，如果@buffer没有设定 #GST_BUFFER_FLAG_TAG_MEMORY，
   *         然后会通知任何正在阻塞状态的 #GstBufferPoolClass::acquire 函数。
   */
  void           (*release_buffer) (GstBufferPool *pool, GstBuffer *buffer);

  /* free这个@buffer，默认的实现是解引用@buffer */
  void           (*free_buffer)    (GstBufferPool *pool, GstBuffer *buffer);

  /* 进入flush状态 */
  void           (*flush_start)    (GstBufferPool *pool);

  /* 离开flushing状态 */
  void           (*flush_stop)     (GstBufferPool *pool);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING - 2];
};
```

#### 2.3.4 GstBufferPoolPrivate
```c
/* filename:gstbufferpool.c */
struct _GstBufferPoolPrivate
{
  GstAtomicQueue *queue;
  GstPoll *poll;

  GRecMutex rec_lock;

  gboolean started;
  gboolean active;
  gint outstanding;             /* number of buffers that are in use */

  gboolean configured;
  GstStructure *config;

  guint size;
  guint min_buffers;
  guint max_buffers;
  guint cur_buffers;
  GstAllocator *allocator;
  GstAllocationParams params;
};
```


## 3 GstBufferPool对象相关函数