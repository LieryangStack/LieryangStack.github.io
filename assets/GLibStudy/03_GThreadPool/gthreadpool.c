#include "config.h"

#include "gthreadpool.h"

#include "gasyncqueue.h"
#include "gasyncqueueprivate.h"
#include "glib-private.h"
#include "gmain.h"
#include "gtestutils.h"
#include "gthreadprivate.h"
#include "gtimer.h"
#include "gutils.h"

/**
 * SECTION:thread_pools
 * @title: 线程池
 * @short_description: 线程池用于并发执行任务
 * @see_also: #GThread
 *
 * 有时候你希望异步地执行工作并在自己的线程中继续工作。如果这种情况经常发生，每次启动和销毁线程的开销可能会太高。
 * 在这种情况下，重用已经启动的线程似乎是个不错的主意。事实上确实如此，但实现这一点可能会很繁琐且容易出错。
 *
 * 因此，GLib 为您提供了线程池以方便您使用。另一个好处是，当不同子系统使用 GLib 时，这些线程可以在程序中的不同部分之间共享。
 *
 * 要创建一个新的线程池，可以使用 g_thread_pool_new()。它通过 g_thread_pool_free() 销毁。
 *
 * 如果要在线程池中执行特定任务，可以调用 g_thread_pool_push()。
 *
 * 要获取当前运行线程的数量，可以调用 g_thread_pool_get_num_threads()。
 * 要获取仍未处理的任务数，可以调用 g_thread_pool_unprocessed()。
 * 要控制线程池的最大线程数，可以使用 g_thread_pool_get_max_threads() 和 g_thread_pool_set_max_threads()。
 *
 * 最后，您可以控制由 GLib 保留以供将来使用的未使用线程的数量。可以使用 g_thread_pool_get_num_unused_threads() 获取当前数量。
 * 可以通过 g_thread_pool_get_max_unused_threads() 和 g_thread_pool_set_max_unused_threads() 控制最大数量。
 * 可以通过调用 g_thread_pool_stop_unused_threads() 停止所有当前未使用的线程。
 */

#define DEBUG_MSG(x)
/* #define DEBUG_MSG(args) g_printerr args ; g_printerr ("\n");    */

typedef struct _GRealThreadPool GRealThreadPool;

/**
 * GThreadPool:
 * @func: 在此线程池的线程中执行的函数
 * @user_data: 此线程池线程的用户数据
 * @exclusive: 是否所有线程都专属于此线程池
 *
 * #GThreadPool 结构体代表一个线程池。它有三个公共的只读成员，但底层的结构体更大，因此你不应该复制此结构体。
 */
struct _GRealThreadPool
{
  GThreadPool pool;
  GAsyncQueue *queue;
  GCond cond;
  gint max_threads;
  guint num_threads;
  gboolean running;
  gboolean immediate;
  gboolean waiting;
  GCompareDataFunc sort_func;
  gpointer sort_user_data;
};

/* 以下只是一个用于标记线程唤醒顺序的地址，它可以是任何地址（只要它不是有效的GThreadPool地址） */
static const gpointer wakeup_thread_marker = (gpointer) &g_thread_pool_new;
static gint wakeup_thread_serial = 0;

/* Here all unused threads are waiting  */
static GAsyncQueue *unused_thread_queue = NULL;
static gint unused_threads = 0;
static gint max_unused_threads = 2;
static gint kill_unused_threads = 0;
static guint max_idle_time = 15 * 1000;

typedef struct
{
  /* Either thread or error are set in the end. Both transfer-full. */
  GThreadPool *pool;
  GThread *thread;
  GError *error;
} SpawnThreadData;

static GCond spawn_thread_cond;
static GAsyncQueue *spawn_thread_queue;

static void             g_thread_pool_queue_push_unlocked (GRealThreadPool  *pool,
                                                           gpointer          data);
static void             g_thread_pool_free_internal       (GRealThreadPool  *pool);
static gpointer         g_thread_pool_thread_proxy        (gpointer          data);
static gboolean         g_thread_pool_start_thread        (GRealThreadPool  *pool,
                                                           GError          **error);
static void             g_thread_pool_wakeup_and_stop_all (GRealThreadPool  *pool);
static GRealThreadPool* g_thread_pool_wait_for_new_pool   (void);
static gpointer         g_thread_pool_wait_for_new_task   (GRealThreadPool  *pool);

static void
g_thread_pool_queue_push_unlocked (GRealThreadPool *pool,
                                   gpointer         data)
{
  if (pool->sort_func)
    g_async_queue_push_sorted_unlocked (pool->queue,
                                        data,
                                        pool->sort_func,
                                        pool->sort_user_data);
  else
    g_async_queue_push_unlocked (pool->queue, data);
}

static GRealThreadPool*
g_thread_pool_wait_for_new_pool (void)
{
  GRealThreadPool *pool;
  gint local_wakeup_thread_serial;
  guint local_max_unused_threads;
  gint local_max_idle_time;
  gint last_wakeup_thread_serial;
  gboolean have_relayed_thread_marker = FALSE;

  local_max_unused_threads = (guint) g_atomic_int_get (&max_unused_threads);
  local_max_idle_time = g_atomic_int_get (&max_idle_time);
  last_wakeup_thread_serial = g_atomic_int_get (&wakeup_thread_serial);

  do
    {
      if ((guint) g_atomic_int_get (&unused_threads) >= local_max_unused_threads)
        {
          /* If this is a superfluous thread, stop it. */
          pool = NULL;
        }
      else if (local_max_idle_time > 0)
        {
          /* If a maximal idle time is given, wait for the given time. */
          DEBUG_MSG (("thread %p waiting in global pool for %f seconds.",
                      g_thread_self (), local_max_idle_time / 1000.0));

          pool = g_async_queue_timeout_pop (unused_thread_queue,
					    local_max_idle_time * 1000);
        }
      else
        {
          /* If no maximal idle time is given, wait indefinitely. */
          DEBUG_MSG (("thread %p waiting in global pool.", g_thread_self ()));
          pool = g_async_queue_pop (unused_thread_queue);
        }

      if (pool == wakeup_thread_marker)
        {
          local_wakeup_thread_serial = g_atomic_int_get (&wakeup_thread_serial);
          if (last_wakeup_thread_serial == local_wakeup_thread_serial)
            {
              if (!have_relayed_thread_marker)
              {
                /* If this wakeup marker has been received for
                 * the second time, relay it.
                 */
                DEBUG_MSG (("thread %p relaying wakeup message to "
                            "waiting thread with lower serial.",
                            g_thread_self ()));

                g_async_queue_push (unused_thread_queue, wakeup_thread_marker);
                have_relayed_thread_marker = TRUE;

                /* If a wakeup marker has been relayed, this thread
                 * will get out of the way for 100 microseconds to
                 * avoid receiving this marker again.
                 */
                g_usleep (100);
              }
            }
          else
            {
              if (g_atomic_int_add (&kill_unused_threads, -1) > 0)
                {
                  pool = NULL;
                  break;
                }

              DEBUG_MSG (("thread %p updating to new limits.",
                          g_thread_self ()));

              local_max_unused_threads = (guint) g_atomic_int_get (&max_unused_threads);
              local_max_idle_time = g_atomic_int_get (&max_idle_time);
              last_wakeup_thread_serial = local_wakeup_thread_serial;

              have_relayed_thread_marker = FALSE;
            }
        }
    }
  while (pool == wakeup_thread_marker);

  return pool;
}

static gpointer
g_thread_pool_wait_for_new_task (GRealThreadPool *pool)
{
  gpointer task = NULL;

  if (pool->running || (!pool->immediate &&
                        g_async_queue_length_unlocked (pool->queue) > 0))
    {
      /* This thread pool is still active. */
      if (pool->max_threads != -1 && pool->num_threads > (guint) pool->max_threads)
        {
          /* This is a superfluous thread, so it goes to the global pool. */
          DEBUG_MSG (("superfluous thread %p in pool %p.",
                      g_thread_self (), pool));
        }
      else if (pool->pool.exclusive)
        {
          /* Exclusive threads stay attached to the pool. */
          task = g_async_queue_pop_unlocked (pool->queue);

          DEBUG_MSG (("thread %p in exclusive pool %p waits for task "
                      "(%d running, %d unprocessed).",
                      g_thread_self (), pool, pool->num_threads,
                      g_async_queue_length_unlocked (pool->queue)));
        }
      else
        {
          /* A thread will wait for new tasks for at most 1/2
           * second before going to the global pool.
           */
          DEBUG_MSG (("thread %p in pool %p waits for up to a 1/2 second for task "
                      "(%d running, %d unprocessed).",
                      g_thread_self (), pool, pool->num_threads,
                      g_async_queue_length_unlocked (pool->queue)));

          task = g_async_queue_timeout_pop_unlocked (pool->queue,
						     G_USEC_PER_SEC / 2);
        }
    }
  else
    {
      /* This thread pool is inactive, it will no longer process tasks. */
      DEBUG_MSG (("pool %p not active, thread %p will go to global pool "
                  "(running: %s, immediate: %s, len: %d).",
                  pool, g_thread_self (),
                  pool->running ? "true" : "false",
                  pool->immediate ? "true" : "false",
                  g_async_queue_length_unlocked (pool->queue)));
    }

  return task;
}

static gpointer
g_thread_pool_spawn_thread (gpointer data)
{
  while (TRUE)
    {
      SpawnThreadData *spawn_thread_data;
      GThread *thread = NULL;
      GError *error = NULL;
      const gchar *prgname = g_get_prgname ();
      gchar name[16] = "pool";

      if (prgname)
        g_snprintf (name, sizeof (name), "pool-%s", prgname);

      g_async_queue_lock (spawn_thread_queue);
      /* Spawn a new thread for the given pool and wake the requesting thread
       * up again with the result. This new thread will have the scheduler
       * settings inherited from this thread and in extension of the thread
       * that created the first non-exclusive thread-pool. */
      spawn_thread_data = g_async_queue_pop_unlocked (spawn_thread_queue);
      thread = g_thread_try_new (name, g_thread_pool_thread_proxy, spawn_thread_data->pool, &error);

      spawn_thread_data->thread = g_steal_pointer (&thread);
      spawn_thread_data->error = g_steal_pointer (&error);

      g_cond_broadcast (&spawn_thread_cond);
      g_async_queue_unlock (spawn_thread_queue);
    }

  return NULL;
}

static gpointer
g_thread_pool_thread_proxy (gpointer data)
{
  GRealThreadPool *pool;

  pool = data;

  DEBUG_MSG (("thread %p started for pool %p.", g_thread_self (), pool));

  g_async_queue_lock (pool->queue);

  while (TRUE)
    {
      gpointer task;

      task = g_thread_pool_wait_for_new_task (pool);
      if (task)
        {
          if (pool->running || !pool->immediate)
            {
              /* A task was received and the thread pool is active,
               * so execute the function.
               */
              g_async_queue_unlock (pool->queue);
              DEBUG_MSG (("thread %p in pool %p calling func.",
                          g_thread_self (), pool));
              pool->pool.func (task, pool->pool.user_data);
              g_async_queue_lock (pool->queue);
            }
        }
      else
        {
          /* No task was received, so this thread goes to the global pool. */
          gboolean free_pool = FALSE;

          DEBUG_MSG (("thread %p leaving pool %p for global pool.",
                      g_thread_self (), pool));
          pool->num_threads--;

          if (!pool->running)
            {
              if (!pool->waiting)
                {
                  if (pool->num_threads == 0)
                    {
                      /* If the pool is not running and no other
                       * thread is waiting for this thread pool to
                       * finish and this is the last thread of this
                       * pool, free the pool.
                       */
                      free_pool = TRUE;
                    }
                  else
                    {
                      /* If the pool is not running and no other
                       * thread is waiting for this thread pool to
                       * finish and this is not the last thread of
                       * this pool and there are no tasks left in the
                       * queue, wakeup the remaining threads.
                       */
                      if (g_async_queue_length_unlocked (pool->queue) ==
                          (gint) -pool->num_threads)
                        g_thread_pool_wakeup_and_stop_all (pool);
                    }
                }
              else if (pool->immediate ||
                       g_async_queue_length_unlocked (pool->queue) <= 0)
                {
                  /* If the pool is not running and another thread is
                   * waiting for this thread pool to finish and there
                   * are either no tasks left or the pool shall stop
                   * immediately, inform the waiting thread of a change
                   * of the thread pool state.
                   */
                  g_cond_broadcast (&pool->cond);
                }
            }

          g_atomic_int_inc (&unused_threads);
          g_async_queue_unlock (pool->queue);

          if (free_pool)
            g_thread_pool_free_internal (pool);

          pool = g_thread_pool_wait_for_new_pool ();
          g_atomic_int_add (&unused_threads, -1);

          if (pool == NULL)
            break;

          g_async_queue_lock (pool->queue);

          DEBUG_MSG (("thread %p entering pool %p from global pool.",
                      g_thread_self (), pool));

          /* pool->num_threads++ is not done here, but in
           * g_thread_pool_start_thread to make the new started
           * thread known to the pool before itself can do it.
           */
        }
    }

  return NULL;
}

static gboolean
g_thread_pool_start_thread (GRealThreadPool  *pool,
                            GError          **error)
{
  gboolean success = FALSE;

  if (pool->max_threads != -1 && pool->num_threads >= (guint) pool->max_threads)
    /* Enough threads are already running */
    return TRUE;

  g_async_queue_lock (unused_thread_queue);

  if (g_async_queue_length_unlocked (unused_thread_queue) < 0)
    {
      g_async_queue_push_unlocked (unused_thread_queue, pool);
      success = TRUE;
    }

  g_async_queue_unlock (unused_thread_queue);

  if (!success)
    {
      const gchar *prgname = g_get_prgname ();
      gchar name[16] = "pool";
      GThread *thread;

      if (prgname)
        g_snprintf (name, sizeof (name), "pool-%s", prgname);

      /* No thread was found, we have to start a new one */
      if (pool->pool.exclusive)
        {
          /* For exclusive thread-pools this is directly called from new() and
           * we simply start new threads that inherit the scheduler settings
           * from the current thread.
           */
          thread = g_thread_try_new (name, g_thread_pool_thread_proxy, pool, error);
        }
      else
        {
          /* For non-exclusive thread-pools this can be called at any time
           * when a new thread is needed. We make sure to create a new thread
           * here with the correct scheduler settings by going via our helper
           * thread.
           */
          SpawnThreadData spawn_thread_data = { (GThreadPool *) pool, NULL, NULL };

          g_async_queue_lock (spawn_thread_queue);

          g_async_queue_push_unlocked (spawn_thread_queue, &spawn_thread_data);

          while (!spawn_thread_data.thread && !spawn_thread_data.error)
            g_cond_wait (&spawn_thread_cond, _g_async_queue_get_mutex (spawn_thread_queue));

          thread = spawn_thread_data.thread;
          if (!thread)
            g_propagate_error (error, g_steal_pointer (&spawn_thread_data.error));
          g_async_queue_unlock (spawn_thread_queue);
        }

      if (thread == NULL)
        return FALSE;

      g_thread_unref (thread);
    }

  /* See comment in g_thread_pool_thread_proxy as to why this is done
   * here and not there
   */
  pool->num_threads++;

  return TRUE;
}

/**
 * g_thread_pool_new:
 * @func: 在新线程池的线程中执行的函数
 * @user_data: 每次调用时传递给 @func 的用户数据
 * @max_threads: 在新线程池中同时执行的最大线程数，-1 表示没有限制
 * @exclusive: 是否应该是独占的线程池？
 * @error: 错误返回位置，或 %NULL
 *
 * 此函数创建一个新的线程池。
 *
 * 每当调用 g_thread_pool_push() 时，要么创建一个新线程，要么重用一个未使用的线程。对于此线程池，最多同时运行 @max_threads 个线程。
 * @max_threads = -1 允许为此线程池创建无限数量的线程。现在，新创建或重用的线程会执行带有两个参数的函数 @func。
 * 第一个参数是传递给 g_thread_pool_push() 的参数，第二个参数是 @user_data。
 *
 * 将 g_get_num_processors() 传递给 @max_threads 可以创建与系统上的逻辑处理器数量相同的线程。这不会将每个线程固定到特定的处理器上。
 *
 * 参数 @exclusive 决定线程池是否独占所有线程，或与其他线程池共享线程。如果 @exclusive 是 %TRUE，则立即启动 @max_threads 个线程，
 * 并且它们将专门为此线程池运行，直到被 g_thread_pool_free() 销毁。如果 @exclusive 是 %FALSE，则在需要时创建线程，并在所有非独占线程池之间共享。
 * 这意味着对于独占线程池，@max_threads 不能为 -1。此外，独占线程池不受 g_thread_pool_set_max_idle_time() 的影响，因为它们的线程永远不会被视为空闲并返回到全局池中。
 *
 * 请注意，独占线程池使用的线程将全部继承当前线程的调度设置，而非独占线程池使用的线程将继承创建此类线程池的第一个线程的调度设置。
 *
 * @error 可以为 %NULL 以忽略错误，或为非 %NULL 以报告错误。只有在 @exclusive 设置为 %TRUE 且无法创建所有 @max_threads 线程时才会出现错误。
 * 请参阅 #GThreadError 可能发生的错误。请注意，即使出现错误，也会返回有效的 #GThreadPool。
 *
 * 返回值：新的 #GThreadPool
 */

GThreadPool *
g_thread_pool_new (GFunc      func,
                   gpointer   user_data,
                   gint       max_threads,
                   gboolean   exclusive,
                   GError   **error)
{
  return g_thread_pool_new_full (func, user_data, NULL, max_threads, exclusive, error);
}

/**
 * g_thread_pool_new_full:
 * @func: 在新线程池的线程中执行的函数
 * @user_data: 每次调用时传递给 @func 的用户数据
 * @item_free_func: （可选）用于传递给 g_async_queue_new_full() 的释放函数
 * @max_threads: 在新线程池中同时执行的最大线程数，`-1` 表示没有限制
 * @exclusive: 是否应该是独占的线程池？
 * @error: 错误返回位置，或 %NULL
 *
 * 此函数创建一个类似于 g_thread_pool_new() 的新线程池，但允许指定 @item_free_func 来释放传递给 g_thread_pool_push() 的数据，
 * 以防止在所有任务被执行之前 #GThreadPool 被停止和释放。
 *
 * 返回值：（完全转移）新的 #GThreadPool
 *
 * 自版本：2.70
 */
GThreadPool *
g_thread_pool_new_full (GFunc           func,
                        gpointer        user_data,
                        GDestroyNotify  item_free_func,
                        gint            max_threads,
                        gboolean        exclusive,
                        GError        **error)
{
  GRealThreadPool *retval;
  G_LOCK_DEFINE_STATIC (init);

  g_return_val_if_fail (func, NULL);
  g_return_val_if_fail (!exclusive || max_threads != -1, NULL);
  g_return_val_if_fail (max_threads >= -1, NULL);

  retval = g_new (GRealThreadPool, 1);

  retval->pool.func = func;
  retval->pool.user_data = user_data;
  retval->pool.exclusive = exclusive;
  retval->queue = g_async_queue_new_full (item_free_func);
  g_cond_init (&retval->cond);
  retval->max_threads = max_threads;
  retval->num_threads = 0;
  retval->running = TRUE;
  retval->immediate = FALSE;
  retval->waiting = FALSE;
  retval->sort_func = NULL;
  retval->sort_user_data = NULL;

  G_LOCK (init);
  if (!unused_thread_queue)
      unused_thread_queue = g_async_queue_new ();

  /*
   * Spawn a helper thread that is only responsible for spawning new threads
   * with the scheduler settings of the current thread.
   *
   * This is then used for making sure that all threads created on the
   * non-exclusive thread-pool have the same scheduler settings, and more
   * importantly don't just inherit them from the thread that just happened to
   * push a new task and caused a new thread to be created.
   *
   * Not doing so could cause real-time priority threads or otherwise
   * threads with problematic scheduler settings to be part of the
   * non-exclusive thread-pools.
   *
   * For exclusive thread-pools this is not required as all threads are
   * created immediately below and are running forever, so they will
   * automatically inherit the scheduler settings from this very thread.
   */
  if (!exclusive && !spawn_thread_queue)
    {
      GThread *pool_spawner = NULL;

      spawn_thread_queue = g_async_queue_new ();
      g_cond_init (&spawn_thread_cond);
      pool_spawner = g_thread_new ("pool-spawner", g_thread_pool_spawn_thread, NULL);
      g_ignore_leak (pool_spawner);
    }
  G_UNLOCK (init);

  if (retval->pool.exclusive)
    {
      g_async_queue_lock (retval->queue);

      while (retval->num_threads < (guint) retval->max_threads)
        {
          GError *local_error = NULL;

          if (!g_thread_pool_start_thread (retval, &local_error))
            {
              g_propagate_error (error, local_error);
              break;
            }
        }

      g_async_queue_unlock (retval->queue);
    }

  return (GThreadPool*) retval;
}


/**
 * g_thread_pool_push:
 * @pool: 一个 #GThreadPool
 * @data: 提交给 @pool 执行的新任务
 * @error: 错误返回位置，或 %NULL
 *
 * 将 @data 插入到要由 @pool 执行的任务列表中。
 *
 * 当当前运行线程的数量低于允许的最大线程数时，会使用给定给 g_thread_pool_new() 的属性启动（或重用）一个新线程。
 * 否则，@data 将保留在队列中，直到此线程池中的线程完成其先前的任务并处理 @data。
 *
 * @error 可以为 %NULL 以忽略错误，或为非 %NULL 以报告错误。只有在无法创建新线程时才会发生错误。
 * 在这种情况下，@data 简单地追加到待执行工作的队列中。
 *
 * 在 2.32 版本之前，此函数没有返回成功状态。
 *
 * 返回值：成功返回 %TRUE，如果发生错误则返回 %FALSE
 */
gboolean
g_thread_pool_push (GThreadPool  *pool,
                    gpointer      data,
                    GError      **error)
{
  GRealThreadPool *real;
  gboolean result;

  real = (GRealThreadPool*) pool;

  g_return_val_if_fail (real, FALSE);
  g_return_val_if_fail (real->running, FALSE);

  result = TRUE;

  g_async_queue_lock (real->queue);

  if (g_async_queue_length_unlocked (real->queue) >= 0)
    {
      /* No thread is waiting in the queue */
      GError *local_error = NULL;

      if (!g_thread_pool_start_thread (real, &local_error))
        {
          g_propagate_error (error, local_error);
          result = FALSE;
        }
    }

  g_thread_pool_queue_push_unlocked (real, data);
  g_async_queue_unlock (real->queue);

  return result;
}

/**
 * g_thread_pool_set_max_threads:
 * @pool: 一个 #GThreadPool
 * @max_threads: @pool 的新最大线程数，或者 -1 表示无限制
 * @error: 错误返回位置，或者 %NULL
 *
 * 设置 @pool 允许的最大线程数。
 * -1 表示最大线程数无限制。如果 @pool 是一个独占线程池，将最大线程数设置为 -1 是不允许的。
 *
 * 将 @max_threads 设置为 0 表示停止 @pool 的所有工作。它会被冻结，直到 @max_threads 再次被设置为非零值。
 *
 * 在调用 g_thread_pool_new() 提供的 @func 时，线程永远不会被终止。相反，最大线程数仅影响 g_thread_pool_push() 中新线程的分配。
 * 只有当 @pool 中当前运行的线程数小于最大线程数时，才会分配新的线程。
 *
 * @error 可以是 %NULL，以忽略错误，也可以是非 %NULL，以报告错误。只有当无法创建新线程时，才会发生错误。
 *
 * 在2.32版本之前，此函数没有返回成功状态。
 *
 * 返回值：在成功时返回 %TRUE，在发生错误时返回 %FALSE
 */
gboolean
g_thread_pool_set_max_threads (GThreadPool  *pool,
                               gint          max_threads,
                               GError      **error)
{
  GRealThreadPool *real;
  gint to_start;
  gboolean result;

  real = (GRealThreadPool*) pool;

  g_return_val_if_fail (real, FALSE);
  g_return_val_if_fail (real->running, FALSE);
  g_return_val_if_fail (!real->pool.exclusive || max_threads != -1, FALSE);
  g_return_val_if_fail (max_threads >= -1, FALSE);

  result = TRUE;

  g_async_queue_lock (real->queue);

  real->max_threads = max_threads;

  if (pool->exclusive)
    to_start = real->max_threads - real->num_threads;
  else
    to_start = g_async_queue_length_unlocked (real->queue);

  for ( ; to_start > 0; to_start--)
    {
      GError *local_error = NULL;

      if (!g_thread_pool_start_thread (real, &local_error))
        {
          g_propagate_error (error, local_error);
          result = FALSE;
          break;
        }
    }

  g_async_queue_unlock (real->queue);

  return result;
}

/**
 * g_thread_pool_get_max_threads:
 * @pool: 一个 #GThreadPool
 *
 * 返回 @pool 的最大线程数。
 *
 * 返回值：最大线程数
 */
gint
g_thread_pool_get_max_threads (GThreadPool *pool)
{
  GRealThreadPool *real;
  gint retval;

  real = (GRealThreadPool*) pool;

  g_return_val_if_fail (real, 0);
  g_return_val_if_fail (real->running, 0);

  g_async_queue_lock (real->queue);
  retval = real->max_threads;
  g_async_queue_unlock (real->queue);

  return retval;
}

/**
 * g_thread_pool_get_num_threads:
 * @pool: 一个 #GThreadPool
 *
 * 返回 @pool 当前正在运行的线程数。
 *
 * 返回值：当前正在运行的线程数
 */
guint
g_thread_pool_get_num_threads (GThreadPool *pool)
{
  GRealThreadPool *real;
  guint retval;

  real = (GRealThreadPool*) pool;

  g_return_val_if_fail (real, 0);
  g_return_val_if_fail (real->running, 0);

  g_async_queue_lock (real->queue);
  retval = real->num_threads;
  g_async_queue_unlock (real->queue);

  return retval;
}

/**
 * g_thread_pool_unprocessed:
 * @pool: 一个 #GThreadPool
 *
 * 返回 @pool 中仍未处理的任务数。
 *
 * 返回值：未处理的任务数
 */
guint
g_thread_pool_unprocessed (GThreadPool *pool)
{
  GRealThreadPool *real;
  gint unprocessed;

  real = (GRealThreadPool*) pool;

  g_return_val_if_fail (real, 0);
  g_return_val_if_fail (real->running, 0);

  unprocessed = g_async_queue_length (real->queue);

  return MAX (unprocessed, 0);
}

/**
 * g_thread_pool_free:
 * @pool: 一个 #GThreadPool
 * @immediate: 是否应立即关闭 @pool？
 * @wait_: 函数是否应等待所有任务完成？
 *
 * 释放为 @pool 分配的所有资源。
 *
 * 如果 @immediate 为 %TRUE，则不会处理 @pool 的新任务。
 * 否则，在最后一个任务处理之前，@pool 不会被释放。
 * 但请注意，在处理任务时不会中断此池的任何线程。相反，至少所有仍在运行的线程
 * 可以在 @pool 被释放之前完成它们的任务。
 *
 * 如果 @wait_ 为 %TRUE，则此函数在所有任务被处理完毕之前（取决于 @immediate，
 * 是所有任务还是只有当前正在运行的任务）不会返回。
 * 否则此函数立即返回。
 *
 * 调用此函数后，@pool 不得再被使用。
 */
void
g_thread_pool_free (GThreadPool *pool,
                    gboolean     immediate,
                    gboolean     wait_)
{
  GRealThreadPool *real;

  real = (GRealThreadPool*) pool;

  g_return_if_fail (real);
  g_return_if_fail (real->running);

  /* If there's no thread allowed here, there is not much sense in
   * not stopping this pool immediately, when it's not empty
   */
  g_return_if_fail (immediate ||
                    real->max_threads != 0 ||
                    g_async_queue_length (real->queue) == 0);

  g_async_queue_lock (real->queue);

  real->running = FALSE;
  real->immediate = immediate;
  real->waiting = wait_;

  if (wait_)
    {
      while (g_async_queue_length_unlocked (real->queue) != (gint) -real->num_threads &&
             !(immediate && real->num_threads == 0))
        g_cond_wait (&real->cond, _g_async_queue_get_mutex (real->queue));
    }

  if (immediate || g_async_queue_length_unlocked (real->queue) == (gint) -real->num_threads)
    {
      /* No thread is currently doing something (and nothing is left
       * to process in the queue)
       */
      if (real->num_threads == 0)
        {
          /* No threads left, we clean up */
          g_async_queue_unlock (real->queue);
          g_thread_pool_free_internal (real);
          return;
        }

      g_thread_pool_wakeup_and_stop_all (real);
    }

  /* The last thread should cleanup the pool */
  real->waiting = FALSE;
  g_async_queue_unlock (real->queue);
}

static void
g_thread_pool_free_internal (GRealThreadPool* pool)
{
  g_return_if_fail (pool);
  g_return_if_fail (pool->running == FALSE);
  g_return_if_fail (pool->num_threads == 0);

  /* Ensure the dummy item pushed on by g_thread_pool_wakeup_and_stop_all() is
   * removed, before it’s potentially passed to the user-provided
   * @item_free_func. */
  g_async_queue_remove (pool->queue, GUINT_TO_POINTER (1));

  g_async_queue_unref (pool->queue);
  g_cond_clear (&pool->cond);

  g_free (pool);
}

static void
g_thread_pool_wakeup_and_stop_all (GRealThreadPool *pool)
{
  guint i;

  g_return_if_fail (pool);
  g_return_if_fail (pool->running == FALSE);
  g_return_if_fail (pool->num_threads != 0);

  pool->immediate = TRUE;

  /*
   * So here we're sending bogus data to the pool threads, which
   * should cause them each to wake up, and check the above
   * pool->immediate condition. However we don't want that
   * data to be sorted (since it'll crash the sorter).
   */
  for (i = 0; i < pool->num_threads; i++)
    g_async_queue_push_unlocked (pool->queue, GUINT_TO_POINTER (1));
}

/**
 * g_thread_pool_set_max_unused_threads:
 * @max_threads: maximal number of unused threads
 *
 * Sets the maximal number of unused threads to @max_threads.
 * If @max_threads is -1, no limit is imposed on the number
 * of unused threads.
 *
 * The default value is 2.
 */
void
g_thread_pool_set_max_unused_threads (gint max_threads)
{
  g_return_if_fail (max_threads >= -1);

  g_atomic_int_set (&max_unused_threads, max_threads);

  if (max_threads != -1)
    {
      max_threads -= g_atomic_int_get (&unused_threads);
      if (max_threads < 0)
        {
          g_atomic_int_set (&kill_unused_threads, -max_threads);
          g_atomic_int_inc (&wakeup_thread_serial);

          g_async_queue_lock (unused_thread_queue);

          do
            {
              g_async_queue_push_unlocked (unused_thread_queue,
                                           wakeup_thread_marker);
            }
          while (++max_threads);

          g_async_queue_unlock (unused_thread_queue);
        }
    }
}

/**
 * g_thread_pool_get_max_unused_threads:
 *
 * Returns the maximal allowed number of unused threads.
 *
 * Returns: the maximal number of unused threads
 */
gint
g_thread_pool_get_max_unused_threads (void)
{
  return g_atomic_int_get (&max_unused_threads);
}

/**
 * g_thread_pool_get_num_unused_threads:
 *
 * Returns the number of currently unused threads.
 *
 * Returns: the number of currently unused threads
 */
guint
g_thread_pool_get_num_unused_threads (void)
{
  return (guint) g_atomic_int_get (&unused_threads);
}

/**
 * g_thread_pool_stop_unused_threads:
 *
 * Stops all currently unused threads. This does not change the
 * maximal number of unused threads. This function can be used to
 * regularly stop all unused threads e.g. from g_timeout_add().
 */
void
g_thread_pool_stop_unused_threads (void)
{
  guint oldval;

  oldval = g_thread_pool_get_max_unused_threads ();

  g_thread_pool_set_max_unused_threads (0);
  g_thread_pool_set_max_unused_threads (oldval);
}

/**
 * g_thread_pool_set_sort_function:
 * @pool: a #GThreadPool
 * @func: the #GCompareDataFunc used to sort the list of tasks.
 *     This function is passed two tasks. It should return
 *     0 if the order in which they are handled does not matter,
 *     a negative value if the first task should be processed before
 *     the second or a positive value if the second task should be
 *     processed first.
 * @user_data: user data passed to @func
 *
 * Sets the function used to sort the list of tasks. This allows the
 * tasks to be processed by a priority determined by @func, and not
 * just in the order in which they were added to the pool.
 *
 * Note, if the maximum number of threads is more than 1, the order
 * that threads are executed cannot be guaranteed 100%. Threads are
 * scheduled by the operating system and are executed at random. It
 * cannot be assumed that threads are executed in the order they are
 * created.
 *
 * Since: 2.10
 */
void
g_thread_pool_set_sort_function (GThreadPool      *pool,
                                 GCompareDataFunc  func,
                                 gpointer          user_data)
{
  GRealThreadPool *real;

  real = (GRealThreadPool*) pool;

  g_return_if_fail (real);
  g_return_if_fail (real->running);

  g_async_queue_lock (real->queue);

  real->sort_func = func;
  real->sort_user_data = user_data;

  if (func)
    g_async_queue_sort_unlocked (real->queue,
                                 real->sort_func,
                                 real->sort_user_data);

  g_async_queue_unlock (real->queue);
}

/**
 * g_thread_pool_move_to_front:
 * @pool: a #GThreadPool
 * @data: an unprocessed item in the pool
 *
 * Moves the item to the front of the queue of unprocessed
 * items, so that it will be processed next.
 *
 * Returns: %TRUE if the item was found and moved
 *
 * Since: 2.46
 */
gboolean
g_thread_pool_move_to_front (GThreadPool *pool,
                             gpointer     data)
{
  GRealThreadPool *real = (GRealThreadPool*) pool;
  gboolean found;

  g_async_queue_lock (real->queue);

  found = g_async_queue_remove_unlocked (real->queue, data);
  if (found)
    g_async_queue_push_front_unlocked (real->queue, data);

  g_async_queue_unlock (real->queue);

  return found;
}

/**
 * g_thread_pool_set_max_idle_time:
 * @interval: the maximum @interval (in milliseconds)
 *     a thread can be idle
 *
 * This function will set the maximum @interval that a thread
 * waiting in the pool for new tasks can be idle for before
 * being stopped. This function is similar to calling
 * g_thread_pool_stop_unused_threads() on a regular timeout,
 * except this is done on a per thread basis.
 *
 * By setting @interval to 0, idle threads will not be stopped.
 *
 * The default value is 15000 (15 seconds).
 *
 * Since: 2.10
 */
void
g_thread_pool_set_max_idle_time (guint interval)
{
  guint i;

  g_atomic_int_set (&max_idle_time, interval);

  i = (guint) g_atomic_int_get (&unused_threads);
  if (i > 0)
    {
      g_atomic_int_inc (&wakeup_thread_serial);
      g_async_queue_lock (unused_thread_queue);

      do
        {
          g_async_queue_push_unlocked (unused_thread_queue,
                                       wakeup_thread_marker);
        }
      while (--i);

      g_async_queue_unlock (unused_thread_queue);
    }
}

/**
 * g_thread_pool_get_max_idle_time:
 *
 * This function will return the maximum @interval that a
 * thread will wait in the thread pool for new tasks before
 * being stopped.
 *
 * If this function returns 0, threads waiting in the thread
 * pool for new work are not stopped.
 *
 * Returns: the maximum @interval (milliseconds) to wait
 *     for new tasks in the thread pool before stopping the
 *     thread
 *
 * Since: 2.10
 */
guint
g_thread_pool_get_max_idle_time (void)
{
  return (guint) g_atomic_int_get (&max_idle_time);
}