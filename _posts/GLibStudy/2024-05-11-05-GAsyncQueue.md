---
layout: post
title: 五、GLib——GAsyncQueue
categories: GLib学习笔记
tags: [GLib]
---

## 1 GAsyncQueue基本概念

- 通常需要在不同的线程之间进行通信。一般来说，最好不要通过共享内存来实现这一点，而是通过显式的消息传递。然而，这些消息只在多线程应用程序中异步地才有意义，因为同步操作同样可以在同一线程中完成。

- 异步队列是GLib大多数其他数据结构的例外，因为它们可以同时从多个线程中使用而无需显式加锁，并且它们带有内置的引用计数。这是因为异步队列的特性是它将始终由至少两个并发线程使用。

- 要使用异步队列，首先必须使用g_async_queue_new()创建一个队列。GAsyncQueue结构是引用计数的，使用g_async_queue_ref()和g_async_queue_unref()来管理你的引用。

- 想要向队列发送消息的线程只需调用g_async_queue_push()将消息推送到队列中。

- 想要从异步队列接收消息的线程只需为该队列调用g_async_queue_pop()。如果在该时刻队列中没有可用的消息，线程将被置于睡眠状态，直到消息到达。消息将从队列中移除并返回。g_async_queue_try_pop()和g_async_queue_timeout_pop()函数可以用于仅检查消息是否存在或仅等待一段时间以获取消息。

- 对于几乎每个函数，都存在两个变体，一个是上锁GMutex队列的，另一个是不上锁GMutex队列的。通过这种方式，你可以在多个队列访问指令中保持队列锁（使用g_async_queue_lock()获取它，并使用g_async_queue_unlock()释放它）。这可能是必要的，以确保队列的完整性，但应仅在确实需要时使用，因为如果不明智地使用，它可能会使你的代码更加困难。通常情况下，你应该只使用不带有_unlocked后缀的不进行上锁函数变体。

**总结**：

<font color="red">
在许多情况下，当你需要将工作分配给一组工作线程时，使用GThreadPool可能更方便，而不是手动使用GAsyncQueue。GThreadPool在内部使用GAsyncQueue实现。
</font>

1. `GAsyncQueue` 没有在类型系统注册，但是自定义结构体存在引用计数。所以有 `g_async_queue_ref` 和 `g_async_queue_ref_unlocked`。（除了创建和解引用，没有其他函数会修改引用计数）

2. `g_async_queue_new` 或者 `g_async_queue_new_full` 创建队列，`g_async_queue_ref` 和 `g_async_queue_ref_unlocked`释放队列。

3. `GAsyncQueue` 相关函数后缀 `_unlocked` 不带有互斥锁，没有该后缀的函数多线程安全。

## 2 GAsyncQueue结构体

```c
/* filename: gasyncQueue.c */
struct _GAsyncQueue
{
  GMutex mutex; /* 互斥锁 */
  GCond cond; /* 条件 */
  GQueue queue; /* 队列 */
  GDestroyNotify item_free_func; /* 释放queue中data的函数 */
  guint waiting_threads; /* 正处于阻塞等待数据线程数量 */
  gint ref_count; /* 引用计数 */
};
```

**引用计数**：

```c
/* 引用计数+1，此时 ref_count == 1 */
GAsyncQueue *g_async_queue_new (void);

/**
 * 只要中间不调用  `g_async_queue_ref` 和 `g_async_queue_ref_unlocked` ，引用计数就不会改变
*/

/* 除了ref函数，没有其他函数能修改引用计数，所以此时 ref_count == 1，调用unref即可释放GAsyncQueue内存 */
g_async_queue_unref (queue);
```

## 3 函数总结

关于函数后缀 `_unlocked` 区别：函数内部是否进行锁操作。

```c

void
g_async_queue_push (GAsyncQueue *queue,
                    gpointer     data) {
  g_mutex_lock (&queue->mutex);
  g_async_queue_push_unlocked (queue, data);
  g_mutex_unlock (&queue->mutex);
}

void
g_async_queue_push_unlocked (GAsyncQueue *queue,
                             gpointer     data) {
  g_queue_push_head (&queue->queue, data);
  if (queue->waiting_threads > 0)
    g_cond_signal (&queue->cond);
}

```

### 3.1 异步队列创建

两个创建函数的区别：是否给data执行数据释放函数 `item_free_func`

```c
GAsyncQueue *
g_async_queue_new (void);

GAsyncQueue *
g_async_queue_new_full (GDestroyNotify item_free_func);
```

### 3.2 异步队列引用与解引用

```c
GAsyncQueue *
g_async_queue_ref (GAsyncQueue *queue);

void
g_async_queue_unref (GAsyncQueue *queue);
```

### 3.3 异步队列数据入队

```c
/* data头部入队 */
void
g_async_queue_push_unlocked (GAsyncQueue *queue,
                             gpointer     data) {
  g_return_if_fail (queue);
  g_return_if_fail (data);

  g_queue_push_head (&queue->queue, data);
  if (queue->waiting_threads > 0)
    g_cond_signal (&queue->cond);
}


/* data尾部入队 */
void
g_async_queue_push_front_unlocked (GAsyncQueue *queue,
                                   gpointer     item) {
  g_return_if_fail (queue != NULL);
  g_return_if_fail (item != NULL);

  g_queue_push_tail (&queue->queue, item);
  if (queue->waiting_threads > 0)
    g_cond_signal (&queue->cond);
}
```

### 3.4 异步队列数据出队

```c
/* data尾部出队 */
gpointer
g_async_queue_pop_unlocked (GAsyncQueue *queue)
{
  g_return_val_if_fail (queue, NULL);

  /* 其中调用的是 g_queue_pop_tail (&queue->queue) */
  return g_async_queue_pop_intern_unlocked (queue, TRUE, -1);
}
```

## 参考

[参考1：Asynchronous Queues
](https://www.manpagez.com/html/glib/glib-2.56.0/glib-Asynchronous-Queues.php)