---
layout: post
title: 八、GLib——GMainContext
categories: GLib学习笔记
tags: [GLib]
---

## 1 GMainContext结构体

```c
/* filename: gmain.h */
typedef struct _GMainContext GMainContext;

/* filename: gmain.c */
struct _GMainContext
{
  /* The following lock is used for both the list of sources
   * and the list of poll records
   */
  GMutex mutex;
  GCond cond;
  GThread *owner; /* 上下文被那个线程拥有 owner = g_thread_self () */
  guint owner_count; 
  GMainContextFlags flags;
  GSList *waiters;

  gint ref_count;  /* (atomic) */

  GHashTable *sources; /* 哈希表会存储该上下文的所有事件源 */

  GPtrArray *pending_dispatches;
  gint timeout;			/* Timeout for current iteration */

  guint next_id;
  GList *source_lists;
  gint in_check_or_prepare;

  GPollRec *poll_records;
  guint n_poll_records;
  GPollFD *cached_poll_array;
  guint cached_poll_array_size;

  GWakeup *wakeup;

  GPollFD wake_up_rec;

/* Flag indicating whether the set of fd's changed during a poll */
  gboolean poll_changed;

  GPollFunc poll_func;

  gint64   time;
  gboolean time_is_fresh;
};
```

**引用计数：**

```c
GMainContext *context = g_main_context_new (); /* 引用计数会加一 context->ref_count = 1 */
GMainLoop *loop = g_main_loop_new(context, TRUE); /* 引用计数会加一 context->ref_count = 2 */

/* 为了程序最后只解引用loop，就可以释放context内存，所以此时context引用减一 */
g_main_context_unref (context);

g_main_loop_run (loop); 

g_main_loop_unref (loop); /* loop解引用的时候，因为loop包含context，所以也会解引用context,因此执行完毕之后 context->ref_count = 0 */
```

## 2 GMainContext相关函数和概念

### 2.1 关于全局默认上下文default_main_context

我们使用以下函数的时候，`g_idle_add`、`g_timeout_add`、`g_main_loop_new`等等和Source相关的函数的时候，我们如果不指定`GMainContext`的时候。调用的都是 `g_main_context_default ()` 函数。

```c
GMainContext *
g_main_context_default (void) {

  static GMainContext *default_main_context = NULL;

  if (g_once_init_enter (&default_main_context))
    {
      GMainContext *context;

      context = g_main_context_new ();

      TRACE (GLIB_MAIN_CONTEXT_DEFAULT (context));

      g_once_init_leave (&default_main_context, context);
    }

  return default_main_context;
}
```

`default_main_context` 这是静态局部变量，只会被执行一次，一个进程只会有一个默认全局上下文。

<font color="red">
1. 我们可以在主线程里面使用默认的全局上下文，其他线程使用自己新建的上下文。
</font>


<font color="red">
2. 默认全局上下文可以通过 g_main_loop_get_context(main) 获取。
</font>

### 2.2 关于线程默认上下文

```c
/* gthread.h */
struct _GPrivate
{
  gpointer       p;
  GDestroyNotify notify;
  gpointer future[2];
};

/* gmain.c */
static GPrivate thread_context_stack = { NULL, free_context_stack, { NULL, NULL };
```

1. 这是线程默认上下文所操作的队列 `thread_context_stack` 所映射的键，他是以键值对的形式，映射着一个队列 ` GQueue *stack = g_queue_new (); `

2. 这个队列可以通过 `GQueue *stack = g_private_get (&thread_context_stack);` 得到。

3. 以下函数不是多线程安全的，而且并不是每个线程会维护一个 `thread_context_stack`，而是所有线程都会使用这个 `thread_context_stack`。

4. 我认为一般不要在线程中使用这个thread_context_stack相关函数，因为它不是被每个线程所维护的。


```c
/**
 * @brief: 尝试成为指定上下文的所有者。
 * @return: 如果其他线程已经是该上下文的所有者，则立即返回 %FALSE
 *          如果该线程是@context上下文的所有者，则返回 %TRUE
 * @note: 所有权是适当递归的：所有者可以再次请求所有权，
 *        并在调用 g_main_context_release() 的次数与 g_main_context_acquire() 的次数相同时释放所有权。
 *        
 *        只要是该线程所有者，调用一次该函数  context->owner_count++;
*/
gboolean 
g_main_context_acquire (GMainContext *context);

/**
 * @brief: context->owner_count-- 
 *         如果 context->count == 0; 则赋值 context->owner = NULL;
*/
void
g_main_context_release (GMainContext *context);


/**
 * @brief: 获取 @context 并将其设置为当前线程的线程默认上下文
 *         这将导致某些异步操作（如大多数基于 [gio][gio] 的 I/O）
 *         在该线程中启动时在 @context 下运行，并将其结果传递到其主循环，
 *         而不是在主线程中的全局默认主上下文下运行。请注意，调用此函数会更改
 *         g_main_context_get_thread_default() 返回的上下文，而不是
 *         g_main_context_default() 返回的上下文，因此它不会影响像 g_idle_add() 这样的函数使用的上下文。
 * 
 * @note: 这会把context从前面入队到thread_context_stack所映射的队列中
*/
void
g_main_context_push_thread_default (GMainContext *context);

/**
 * @brief: 把@context从thread_context_stack所映射的队列中从前面出队
 * @note: 这会判断 thread_context_stack所映射的队列第一个元素是否等于 @context
*/
void
g_main_context_pop_thread_default (GMainContext *context);

/**
 * @brief: 得到thread_context_stack所映射的队列中第一个元素
*/
GMainContext *
g_main_context_get_thread_default (void);

/**
 * @brief: 将thread_context_stack所映射的队列中第一个元素引用计数值加一
*/
GMainContext *
g_main_context_ref_thread_default (void);
```

### 2.3 GMainContext常用函数

```c
/* 创建GMainContext对象 */
GMainContext *
g_main_context_new (void);

/* GMainContext对象引用计数加一 */
GMainContext *
g_main_context_ref (GMainContext *context);

/* GMainContext对象引用计数减一 */
void
g_main_context_unref (GMainContext *context);
```




```c


```



```c

```



```c

```



```c

```


```




```c


```



```c

```



```c

```



```c

```