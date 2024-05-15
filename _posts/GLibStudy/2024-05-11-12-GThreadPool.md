---
layout: post
title: 十二、GLib——GThreadPool
categories: GLib学习笔记
tags: [GLib]
---

## 1 GThreadPool基本概念

- 有时你希望异步地派发工作的执行，然后继续在自己的线程中工作。如果这种情况经常发生，每次启动和销毁线程的开销可能会太高。在这种情况下，重用已经启动的线程似乎是个不错的主意。确实如此，但是实现这一点可能很繁琐且容易出错。因此，GLib提供了线程池以方便你使用。另一个优势是，当不同的子系统使用GLib时，线程可以在它们之间共享。

- 要创建一个新的线程池，你可以使用g_thread_pool_new()。它可以通过g_thread_pool_free()来销毁。

- 如果你想在线程池中执行某个特定任务，可以调用g_thread_pool_push()。

- 要获取当前正在运行的线程数，可以调用g_thread_pool_get_num_threads()。

- 要获取仍未处理任务的数量，可以调用g_thread_pool_unprocessed()。

- 要控制线程池的最大线程数，可以使用g_thread_pool_get_max_threads()和g_thread_pool_set_max_threads()。

- 最后，你可以控制由GLib保留的未使用线程的数量。当前数量可以通过g_thread_pool_get_num_unused_threads()获取。最大数量可以通过g_thread_pool_get_max_unused_threads()和g_thread_pool_set_max_unused_threads()进行控制。可以通过调用g_thread_pool_stop_unused_threads()停止所有当前未使用的线程。

## 2 GThreadPool相关函数

### 2.1 g_thread_pool_new ()

```c
/**
 * @brief: 创建线程池对象
 * @func: 一个线程池对象其实只能运行一个函数，该变量就是线程池的线程中执行的函数，只不过线程执行的函数可以有不同的参数。
 * @user_data: 每次调用时传递给 @func 的用户数据。
 * @max_threads: 在新线程池中同时执行的最大线程数，-1 表示没有限制（独占线程池的时候，不能设置-1，因为独占线程池的时候，会立即创建相应的数量的线程）
 * @exclusive: 是否应该是独占的线程池
 * @error: 错误返回位置，或 %NULL
 */
GThreadPool *
g_thread_pool_new (GFunc      func,
                   gpointer   user_data,
                   gint       max_threads,
                   gboolean   exclusive,
                   GError   **error);
```

#### 2.1.1 关于exclusiv独占池的概念


## 参考
[参考1：GNome Developer,Thread Pools](https://developer-old.gnome.org/glib/stable/glib-Thread-Pools.html)