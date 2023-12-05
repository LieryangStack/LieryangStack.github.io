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
struct _GstTaskPoolClass {
  GstObjectClass parent_class;

  /*< public >*/
  /* 默认的prepare函数就是创建一个GThreadPool对象，准备一个线程池对象 
   * 使用 gst_task_pool_prepare 函数调用此虚函数
   */
  void      (*prepare)  (GstTaskPool *pool, GError **error);
  
  /* 默认的cleanup函数就是释放GThreadPool对象 
   * 使用 gst_task_pool_cleanup 函数调用此虚函数
   */
  void      (*cleanup)  (GstTaskPool *pool);

  /* 从线程池中执行@func */
  gpointer  (*push)     (GstTaskPool *pool, GstTaskPoolFunction func,
                         gpointer user_data, GError **error);

  /* 检查@func是否完成之类的操作，默认的join什么都没有做 */
  void      (*join)     (GstTaskPool *pool, gpointer id);

  /* 用来释放 GstTaskPoolClass::push 的返回值*/
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