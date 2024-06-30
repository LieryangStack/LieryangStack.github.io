---
layout: post
title: 十六、GstTaskPool
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstTaskPool基本概念

- GstTaskPool中提供了两种对象，`GstTaskPool` 和 `GstSharedTaskPool`，其中共享任务池仅仅在线程池中创建了一个线程（最大可执行线程数是1），而 `GstTaskPool` 没有同时最大可执行线程数量限制。

- 创建GstTaskPool对象后，必须要使用 `gst_task_pool_prepare` 函数创建线程池，否则，没有线程池执行任务函数。

## 2 GstTaskPool类型结构

![alt text](/assets/GStreamerCoreObject/16_GstTaskPool/image/image-3.png)

### 2.1 GstTaskPool结构分析

![alt text](/assets/GStreamerCoreObject/16_GstTaskPool/image/image-1.png)

### 2.2 GstTaskPool相关函数总结

```c
/**
 * @brief: 创建一个GstTaskPool对象
 * @note: 此时并没有创建 GstTaskPool->pool 的GThreadPool对象，必须要调用prepare函数
*/
GstTaskPool *
gst_task_pool_new (void);


/**
 * @brief: 创建了 GstTaskPool->pool 的GThreadPool对象
 * @note: 此时才可以调用 gst_task_pool_push 函数，使用线程池处理线程任务
 *        使用完毕后，必须使用 gst_task_pool_cleanup 释放线程池
*/
void
gst_task_pool_prepare (GstTaskPool * pool, GError ** error);


/**
 * @name: gst_task_pool_push
 * @param pool: 一个 #GstTaskPool
 * @param func: 异步调用的函数，该函数会在线程池中被调用
 * @param user_data: 传递给 @func 的数据
 * @param error: 用于返回发生错误的位置
 * @brief: 从@pool中开始执行一个新线程
 * @return: 一个指针，应该用于 gst_task_pool_join 函数（但是系统默认没有实现join函数）
*/
gpointer
gst_task_pool_push (GstTaskPool * pool, GstTaskPoolFunction func,
    gpointer user_data, GError ** error);


/**
 * @name: gst_task_pool_cleanup
 * @brief: 释放 GstTaskPool->pool 的GThreadPool对象
*/
void
gst_task_pool_cleanup (GstTaskPool * pool);
```

## 3 GstTaskPool示例程序

![alt text](/assets/GStreamerCoreObject/16_GstTaskPool/image/image-2.png)

### 3.1 线程数量分析

下图中，一共有三个线程。其中：

1. `GstTaskPoolStud` 应用程序主线程 main 函数线程是每个程序都会有的。

2. `pool-spawner` 这是线程池的调度线程，用于调用线程处理push传入的任务。只要使用线程池就会有该函数。

3. `pool-GstTaskpoo` 这是线程池中执行任务的线程，该线程的数量跟同时要处理的任务数量有关。如果同时push多少个任务，就能同时新建多少个线程处理任务。（最大同时处理线程数量在线程池中设定）

![alt text](/assets/GStreamerCoreObject/16_GstTaskPool/image/image-4.png)


