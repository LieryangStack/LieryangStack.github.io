#include "gst_private.h"

#include "gstinfo.h"
#include "gsttaskpool.h"
#include "gsterror.h"

GST_DEBUG_CATEGORY_STATIC (taskpool_debug);
#define GST_CAT_DEFAULT (taskpool_debug)

#ifndef GST_DISABLE_GST_DEBUG
static void gst_task_pool_finalize (GObject * object);
#endif

#define _do_init \
{ \
  GST_DEBUG_CATEGORY_INIT (taskpool_debug, "taskpool", 0, "Thread pool"); \
}

G_DEFINE_TYPE_WITH_CODE (GstTaskPool, gst_task_pool, GST_TYPE_OBJECT, _do_init);

typedef struct
{
  GstTaskPoolFunction func;
  gpointer user_data;
} TaskData;

/**
 * @brief: 线程池默认执行函数
 * @param tdata: push函数传入的数据
 * @param pool: new新建线程池的时候传入的数据
*/
static void
default_func (TaskData * tdata, GstTaskPool * pool)
{
  GstTaskPoolFunction func;
  gpointer user_data;

  func = tdata->func;
  user_data = tdata->user_data;
  g_slice_free (TaskData, tdata);

  func (user_data);
}

static void
default_prepare (GstTaskPool * pool, GError ** error)
{
  GST_OBJECT_LOCK (pool);
  pool->pool = g_thread_pool_new ((GFunc) default_func, pool, -1, FALSE, error);
  GST_OBJECT_UNLOCK (pool);
}

static void
default_cleanup (GstTaskPool * pool)
{
  GThreadPool *pool_;

  GST_OBJECT_LOCK (pool);
  pool_ = pool->pool;
  pool->pool = NULL;
  GST_OBJECT_UNLOCK (pool);

  if (pool_) {
    /* Shut down all the threads, we still process the ones scheduled
     * because the unref happens in the thread function.
     * Also wait for currently running ones to finish. */
    g_thread_pool_free (pool_, FALSE, TRUE);
  }
}

static gpointer
default_push (GstTaskPool * pool, GstTaskPoolFunction func,
    gpointer user_data, GError ** error)
{
  TaskData *tdata;

  tdata = g_slice_new (TaskData);
  tdata->func = func;
  tdata->user_data = user_data;

  GST_OBJECT_LOCK (pool);
  if (pool->pool)
    g_thread_pool_push (pool->pool, tdata, error);
  else {
    g_slice_free (TaskData, tdata);
    g_set_error_literal (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
        "No thread pool");

  }
  GST_OBJECT_UNLOCK (pool);

  return NULL;
}

static void
default_join (GstTaskPool * pool, gpointer id)
{
  /* we do nothing here, we can't join from the pools */
}

static void
default_dispose_handle (GstTaskPool * pool, gpointer id)
{
  /* we do nothing here, the default handle is NULL */
}

static void
gst_task_pool_class_init (GstTaskPoolClass * klass)
{
  GObjectClass *gobject_class;
  GstTaskPoolClass *gsttaskpool_class;

  gobject_class = (GObjectClass *) klass;
  gsttaskpool_class = (GstTaskPoolClass *) klass;

#ifndef GST_DISABLE_GST_DEBUG
  gobject_class->finalize = gst_task_pool_finalize;
#endif

  gsttaskpool_class->prepare = default_prepare;
  gsttaskpool_class->cleanup = default_cleanup;
  gsttaskpool_class->push = default_push;
  gsttaskpool_class->join = default_join;
  gsttaskpool_class->dispose_handle = default_dispose_handle;
}

static void
gst_task_pool_init (GstTaskPool * pool)
{
}

#ifndef GST_DISABLE_GST_DEBUG
static void
gst_task_pool_finalize (GObject * object)
{
  GST_DEBUG ("taskpool %p finalize", object);

  G_OBJECT_CLASS (gst_task_pool_parent_class)->finalize (object);
}
#endif

/* 创建一个新的默认任务池。默认任务池将使用常规的 GThreadPool 来创建线程。 */
GstTaskPool *
gst_task_pool_new (void)
{
  GstTaskPool *pool;

  pool = g_object_new (GST_TYPE_TASK_POOL, NULL);

  /* clear floating flag */
  gst_object_ref_sink (pool);

  return pool;
}


/* 准备taskpool以接受 gst_task_pool_push 操作 
 * 该函数多线程安全
 */
void
gst_task_pool_prepare (GstTaskPool * pool, GError ** error)
{
  GstTaskPoolClass *klass;

  g_return_if_fail (GST_IS_TASK_POOL (pool));

  klass = GST_TASK_POOL_GET_CLASS (pool);

  if (klass->prepare)
    klass->prepare (pool, error);
}


/**
 * @name: gst_task_pool_cleanup
 * @brief: 等等所有tasks都被停止，这主要在内部使用确保正确清理测试套件中的内部数据结构。
*/
void
gst_task_pool_cleanup (GstTaskPool * pool)
{
  GstTaskPoolClass *klass;

  g_return_if_fail (GST_IS_TASK_POOL (pool));

  klass = GST_TASK_POOL_GET_CLASS (pool);

  if (klass->cleanup)
    klass->cleanup (pool);
}


/**
 * @name: gst_task_pool_push
 * @param pool: 一个 #GstTaskPool
 * @param func: 异步调用的函数
 * @param user_data: 传递给 @func 的数据
 * @param error: 用于返回发生错误的位置
 * @brief: 从@pool中开始执行一个新线程
 * @return: 一个指针，应该用于 gst_task_pool_join 函数。
 * @note: 这个返回值一般是给 gst_task_pool_join () 函数的第二个参数使用。
 *        如果@return不是NULL，并且没有使用 gst_task_pool_join(),请使用gst_task_pool_dispose_handle()
 *        必须检查@error是否发生错误。
*/
gpointer
gst_task_pool_push (GstTaskPool * pool, GstTaskPoolFunction func,
    gpointer user_data, GError ** error)
{
  GstTaskPoolClass *klass;

  g_return_val_if_fail (GST_IS_TASK_POOL (pool), NULL);

  klass = GST_TASK_POOL_GET_CLASS (pool);

  if (klass->push == NULL)
    goto not_supported;

  return klass->push (pool, func, user_data, error);

  /* ERRORS */
not_supported:
  {
    g_warning ("pushing tasks on pool %p is not supported", pool);
    return NULL;
  }
}


/**
 * @name: gst_task_pool_join
 * @param pool: 一个 #GstTask对象
 * @param id: 任务的ID
 * @brief: 加入一个任务，或者将其返回到池中。@id是从 gst_task_pool_push() 函数返回值获取到。
 *         源代码中的默认实现什么也没有做，因为默认的 #GstTaskPoolClass::push 实现始终返回 %NULL。
*/
void
gst_task_pool_join (GstTaskPool * pool, gpointer id)
{
  GstTaskPoolClass *klass;

  g_return_if_fail (GST_IS_TASK_POOL (pool));

  klass = GST_TASK_POOL_GET_CLASS (pool);

  if (klass->join)
    klass->join (pool, id);
}

/**
 * gst_task_pool_dispose_handle:
 * @pool: 一个 #GstTaskPool
 * @id: （完全转移）（可为空）：任务的 ID
 *
 * 处置由 gst_task_pool_push() 返回的句柄。对于默认实现，这不需要调用，因为默认的 #GstTaskPoolClass::push 实现始终返回 %NULL。
 * 在调用 gst_task_pool_join() 时也不需要调用此方法，但当不需要gst_task_pool_join () 但 gst_task_pool_push() 返回了非 %NULL 值时，应该调用此方法。
 *
 * 这个方法只应该使用与提供 @id 的相同 @pool 实例。
 *
 * 自版本：1.20
 */
void
gst_task_pool_dispose_handle (GstTaskPool * pool, gpointer id)
{
  GstTaskPoolClass *klass;

  g_return_if_fail (GST_IS_TASK_POOL (pool));

  klass = GST_TASK_POOL_GET_CLASS (pool);

  if (klass->dispose_handle)
    klass->dispose_handle (pool, id);
}

typedef struct
{
  gboolean done;
  guint64 id;
  GstTaskPoolFunction func;
  gpointer user_data;
  GMutex done_lock;
  GCond done_cond;
  gint refcount;
} SharedTaskData;

static SharedTaskData *
shared_task_data_ref (SharedTaskData * tdata)
{
  g_atomic_int_add (&tdata->refcount, 1);

  return tdata;
}

static void
shared_task_data_unref (SharedTaskData * tdata)
{
  if (g_atomic_int_dec_and_test (&tdata->refcount)) {
    g_mutex_clear (&tdata->done_lock);
    g_cond_clear (&tdata->done_cond);
    g_slice_free (SharedTaskData, tdata);
  }
}

struct _GstSharedTaskPoolPrivate
{
  guint max_threads;
};

#define GST_SHARED_TASK_POOL_CAST(pool)       ((GstSharedTaskPool*)(pool))

G_DEFINE_TYPE_WITH_PRIVATE (GstSharedTaskPool, gst_shared_task_pool,
    GST_TYPE_TASK_POOL);

static void
shared_func (SharedTaskData * tdata, GstTaskPool * pool)
{
  tdata->func (tdata->user_data);

  g_mutex_lock (&tdata->done_lock);
  tdata->done = TRUE;
  g_cond_signal (&tdata->done_cond);
  g_mutex_unlock (&tdata->done_lock);

  shared_task_data_unref (tdata);
}

static gpointer
shared_push (GstTaskPool * pool, GstTaskPoolFunction func,
    gpointer user_data, GError ** error)
{
  SharedTaskData *ret = NULL;

  GST_OBJECT_LOCK (pool);

  if (!pool->pool) {
    GST_OBJECT_UNLOCK (pool);
    goto done;
  }

  ret = g_slice_new (SharedTaskData);

  ret->done = FALSE;
  ret->func = func;
  ret->user_data = user_data;
  g_atomic_int_set (&ret->refcount, 1);
  g_cond_init (&ret->done_cond);
  g_mutex_init (&ret->done_lock);

  g_thread_pool_push (pool->pool, shared_task_data_ref (ret), error);

  GST_OBJECT_UNLOCK (pool);

done:
  return ret;
}

static void
shared_join (GstTaskPool * pool, gpointer id)
{
  SharedTaskData *tdata;

  if (!id)
    return;

  tdata = (SharedTaskData *) id;

  g_mutex_lock (&tdata->done_lock);
  while (!tdata->done) {
    g_cond_wait (&tdata->done_cond, &tdata->done_lock);
  }
  g_mutex_unlock (&tdata->done_lock);

  shared_task_data_unref (tdata);
}

static void
shared_dispose_handle (GstTaskPool * pool, gpointer id)
{
  SharedTaskData *tdata;

  if (!id)
    return;

  tdata = (SharedTaskData *) id;


  shared_task_data_unref (tdata);
}

static void
shared_prepare (GstTaskPool * pool, GError ** error)
{
  GstSharedTaskPool *shared_pool = GST_SHARED_TASK_POOL_CAST (pool);

  GST_OBJECT_LOCK (pool);
  pool->pool =
      g_thread_pool_new ((GFunc) shared_func, pool,
      shared_pool->priv->max_threads, FALSE, error);
  GST_OBJECT_UNLOCK (pool);
}

static void
gst_shared_task_pool_class_init (GstSharedTaskPoolClass * klass)
{
  GstTaskPoolClass *taskpoolclass = GST_TASK_POOL_CLASS (klass);

  taskpoolclass->prepare = shared_prepare;
  taskpoolclass->push = shared_push;
  taskpoolclass->join = shared_join;
  taskpoolclass->dispose_handle = shared_dispose_handle;
}

static void
gst_shared_task_pool_init (GstSharedTaskPool * pool)
{
  GstSharedTaskPoolPrivate *priv;

  priv = pool->priv = gst_shared_task_pool_get_instance_private (pool);
  priv->max_threads = 1;
}

/**
 * gst_shared_task_pool_set_max_threads:
 * @pool: a #GstSharedTaskPool
 * @max_threads: Maximum number of threads to spawn.
 *
 * Update the maximal number of threads the @pool may spawn. When
 * the maximal number of threads is reduced, existing threads are not
 * immediately shut down, see g_thread_pool_set_max_threads().
 *
 * Setting @max_threads to 0 effectively freezes the pool.
 *
 * Since: 1.20
 */
void
gst_shared_task_pool_set_max_threads (GstSharedTaskPool * pool,
    guint max_threads)
{
  GstTaskPool *taskpool;

  g_return_if_fail (GST_IS_SHARED_TASK_POOL (pool));

  taskpool = GST_TASK_POOL (pool);

  GST_OBJECT_LOCK (pool);
  if (taskpool->pool)
    g_thread_pool_set_max_threads (taskpool->pool, max_threads, NULL);
  pool->priv->max_threads = max_threads;
  GST_OBJECT_UNLOCK (pool);
}

/**
 * gst_shared_task_pool_get_max_threads:
 * @pool: a #GstSharedTaskPool
 *
 * Returns: the maximum number of threads @pool is configured to spawn
 * Since: 1.20
 */
guint
gst_shared_task_pool_get_max_threads (GstSharedTaskPool * pool)
{
  guint ret;

  g_return_val_if_fail (GST_IS_SHARED_TASK_POOL (pool), 0);

  GST_OBJECT_LOCK (pool);
  ret = pool->priv->max_threads;
  GST_OBJECT_UNLOCK (pool);

  return ret;
}

/**
 * gst_shared_task_pool_new:
 *
 * 创建一个新的共享任务池。共享任务池将任务排队在最大数量的线程上，默认为1个线程。
 *
 * 不要使用 #GstSharedTaskPool 来管理可能相互依赖的任务，比如 pad tasks（端口任务），
 * 因为如果一个任务等待另一个任务返回后才能返回，如果它们恰好共享同一个线程，就会导致明显的死锁问题。
 *
 * 返回值: （完全转移）一个新的 #GstSharedTaskPool。在使用后请使用 gst_object_unref() 释放。
 * 自：1.20版本起
 */
GstTaskPool *
gst_shared_task_pool_new (void)
{
  GstTaskPool *pool;

  pool = g_object_new (GST_TYPE_SHARED_TASK_POOL, NULL);

  /* clear floating flag */
  gst_object_ref_sink (pool);

  return pool;
}