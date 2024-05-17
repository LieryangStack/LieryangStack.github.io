---
layout: post
title: 一、GIO——GCancellable
categories: GIO学习笔记
tags: [GIO]
---


GCancellable是GIO中广泛使用的一个线程安全的操作取消堆栈，用于允许取消同步和异步操作。

1. 这是继承于 `GObject` 的标准对象。

## 1 GCancellable对象
/* filename: gcancellable.h */
```c
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
  gboolean cancelled;
  /* Access to fields below is protected by cancellable_mutex. */
  guint cancelled_running : 1;
  guint cancelled_running_waiting : 1;
  unsigned cancelled_emissions;
  unsigned cancelled_emissions_waiting : 1;

  guint fd_refcount;
  GWakeup *wakeup;
};

```

## GCancellable相关函数

```c
gboolean
g_cancellable_is_cancelled (GCancellable *cancellable)
{
  return cancellable != NULL && g_atomic_int_get (&cancellable->priv->cancelled);
}



``` 


会发出取消信号，