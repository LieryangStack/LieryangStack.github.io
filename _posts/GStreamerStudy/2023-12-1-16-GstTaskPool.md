---
layout: post
title: 十六、GstTaskPool
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstTaskPool基本概念

- 此对象提供了创建线程的抽象。默认实现使用常规的 GThreadPool 来启动任务。

- 子类可以创建自定义线程。


## 2 GstTaskPool类型结构

### 2.1 GstTaskPool类型注册宏定义

```c
/* filename: gsttaskpool.h */
#define GST_TYPE_TASK_POOL             (gst_task_pool_get_type ())

/* filename: gsttaskpool.c */
G_DEFINE_TYPE_WITH_CODE (GstTaskPool, gst_task_pool, GST_TYPE_OBJECT, _do_init);
```

### 2.2 GstTaskPool相关结构体

#### 2.2.1 GstTaskPool

```c
/* filename: gsttaskpool.h */
/**
 * GstTaskPool:
 *
 * The #GstTaskPool object.
 */
struct _GstTaskPool {
  GstObject      object;

  /*< private >*/
  GThreadPool   *pool;

  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.2.2 GstTaskPoolClass

```c
/* filename: gsttaskpool.h */
/**
 * GstTaskPoolClass:
 * @parent_class: the parent class structure
 * @prepare: prepare the threadpool
 * @cleanup: make sure all threads are stopped
 * @push: start a new thread
 * @join: join a thread
 *
 * The #GstTaskPoolClass object.
 */
struct _GstTaskPoolClass {
  GstObjectClass parent_class;

  /*< public >*/
  void      (*prepare)  (GstTaskPool *pool, GError **error);
  void      (*cleanup)  (GstTaskPool *pool);

  gpointer  (*push)     (GstTaskPool *pool, GstTaskPoolFunction func,
                         gpointer user_data, GError **error);
  void      (*join)     (GstTaskPool *pool, gpointer id);

  /**
   * GstTaskPoolClass::dispose_handle:
   * @pool: a #GstTaskPool
   * @id: (transfer full): the handle to dispose of
   *
   * free / unref the handle returned in GstTaskPoolClass::push.
   *
   * Since: 1.20
   */
  void      (*dispose_handle) (GstTaskPool *pool, gpointer id);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING - 1];
};
```

#### 2.2.3 TaskData
```c
/* filename: gsttaskpool.c */
typedef struct
{
  GstTaskPoolFunction func;
  gpointer user_data;
} TaskData;
```


## 3 GstTaskPool对象相关函数