---
layout: post
title: 一、GIO——GCancellable
categories: GIO学习笔记
tags: [GIO]
---


GCancellable是GIO中广泛使用的一个线程安全的操作取消堆栈，用于允许取消同步和异步操作。

1. 这是继承于 `GObject` 的标准对象。

2. 可以使用该对象到所有线程中，可以统一退出所有线程。

## 1 GCancellable对象

```c
/* filename: gcancellable.h */
struct _GCancellable
{
  GObject parent_instance;

  /*< private >*/
  GCancellablePrivate *priv;
};

struct _GCancellableClass
{
  GObjectClass parent_class;

  void (* cancelled) (GCancellable *cancellable);

  /*< private >*/
  /* Padding for future expansion */
  void (*_g_reserved1) (void);
  void (*_g_reserved2) (void);
  void (*_g_reserved3) (void);
  void (*_g_reserved4) (void);
  void (*_g_reserved5) (void);
};

/* filename: gcancellable.c */
struct _GCancellablePrivate
{
  /* Atomic so that g_cancellable_is_cancelled does not require holding the mutex. */
  gboolean cancelled; /* 是否取消flag */
  /* Access to fields below is protected by cancellable_mutex. */
  guint cancelled_running : 1;
  guint cancelled_running_waiting : 1;
  unsigned cancelled_emissions;
  unsigned cancelled_emissions_waiting : 1;

  guint fd_refcount;
  GWakeup *wakeup;
};

```

## 2 GCancellable相关函数

```c
/* 创建GCancellable对象 */
GCancellable *
g_cancellable_new (void);

/* 判断可取消对象，是否设置了取消 */
gboolean
g_cancellable_is_cancelled (GCancellable *cancellable)
{
  return cancellable != NULL && g_atomic_int_get (&cancellable->priv->cancelled);
}

/* 设置可取消对象到取消 */
void
g_cancellable_cancel (GCancellable *cancellable);

/* "cancelled" 信号连接回调函数 */
gulong
g_cancellable_connect (GCancellable   *cancellable,
                       GCallback       callback,
                       gpointer        data,
                       GDestroyNotify  data_destroy_func);

/* "cancelled" 删除信号连接回调函数 */
void
g_cancellable_disconnect (GCancellable  *cancellable,
			                    gulong         handler_id);
``` 
