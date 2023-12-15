#include "gst_private.h"
#include "glib-compat-private.h"

#include <errno.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <sys/types.h>

#include "gstatomicqueue.h"
#include "gstpoll.h"
#include "gstinfo.h"
#include "gstquark.h"
#include "gstvalue.h"

#include "gstbufferpool.h"

#ifdef G_OS_WIN32
#  ifndef EWOULDBLOCK
#  define EWOULDBLOCK EAGAIN    /* This is just to placate gcc */
#  endif
#endif /* G_OS_WIN32 */

/* lieryang add */
#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC (gst_buffer_pool_debug);
#define GST_CAT_DEFAULT gst_buffer_pool_debug

#define GST_BUFFER_POOL_LOCK(pool)   (g_rec_mutex_lock(&pool->priv->rec_lock))
#define GST_BUFFER_POOL_UNLOCK(pool) (g_rec_mutex_unlock(&pool->priv->rec_lock))

struct _GstBufferPoolPrivate
{
  GstAtomicQueue *queue;
  GstPoll *poll;

  GRecMutex rec_lock;

  gboolean started;
  gboolean active;
  gint outstanding;/* 正在被使用buffer是数量（未归还Poolbuffer的数量） */ 

  gboolean configured;
  GstStructure *config; /* config配置存储 */

  guint size;
  guint min_buffers;
  guint max_buffers;
  guint cur_buffers; /* pool已经创建了多少个buffer */
  GstAllocator *allocator;
  GstAllocationParams params;
};

static void gst_buffer_pool_dispose (GObject * object);
static void gst_buffer_pool_finalize (GObject * object);

G_DEFINE_TYPE_WITH_PRIVATE (GstBufferPool, gst_buffer_pool, GST_TYPE_OBJECT);

static gboolean default_start (GstBufferPool * pool);
static gboolean default_stop (GstBufferPool * pool);
static gboolean default_set_config (GstBufferPool * pool,
    GstStructure * config);
static GstFlowReturn default_alloc_buffer (GstBufferPool * pool,
    GstBuffer ** buffer, GstBufferPoolAcquireParams * params);
static GstFlowReturn default_acquire_buffer (GstBufferPool * pool,
    GstBuffer ** buffer, GstBufferPoolAcquireParams * params);
static void default_reset_buffer (GstBufferPool * pool, GstBuffer * buffer);
static void default_free_buffer (GstBufferPool * pool, GstBuffer * buffer);
static void default_release_buffer (GstBufferPool * pool, GstBuffer * buffer);

static void
gst_buffer_pool_class_init (GstBufferPoolClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;

  gobject_class->dispose = gst_buffer_pool_dispose;
  gobject_class->finalize = gst_buffer_pool_finalize;

  /*没有klass->get_options*/
  klass->set_config = default_set_config;
  klass->start = default_start;
  klass->stop = default_stop;
  klass->acquire_buffer = default_acquire_buffer;
  klass->alloc_buffer = default_alloc_buffer;
  klass->reset_buffer = default_reset_buffer;
  klass->release_buffer = default_release_buffer;
  klass->free_buffer = default_free_buffer;
  /*没有klass->flush_start*/
  /*没有klass->flush_stop*/

  GST_DEBUG_CATEGORY_INIT (gst_buffer_pool_debug, "bufferpool", 0,
      "bufferpool debug");
}

static void
gst_buffer_pool_init (GstBufferPool * pool)
{
  GstBufferPoolPrivate *priv;

  priv = pool->priv = gst_buffer_pool_get_instance_private (pool);

  g_rec_mutex_init (&priv->rec_lock);

  priv->poll = gst_poll_new_timer ();  /* 创建了一个定时器poll轮询对象 */
  priv->queue = gst_atomic_queue_new (16);
  pool->flushing = 1;
  priv->active = FALSE;
  priv->configured = FALSE;
  priv->started = FALSE;
  priv->config = gst_structure_new_id_empty (GST_QUARK (BUFFER_POOL_CONFIG));
  gst_buffer_pool_config_set_params (priv->config, NULL, 0, 0, 0);
  priv->allocator = NULL;
  gst_allocation_params_init (&priv->params);
  gst_buffer_pool_config_set_allocator (priv->config, priv->allocator,
      &priv->params);
  /* 1 control write for flushing - the flush token */
  gst_poll_write_control (priv->poll);
  /* 1 control write for marking that we are not waiting for poll - the wait token */
  gst_poll_write_control (priv->poll);

  GST_DEBUG_OBJECT (pool, "created");
}

static void
gst_buffer_pool_dispose (GObject * object)
{
  GstBufferPool *pool;
  GstBufferPoolPrivate *priv;

  pool = GST_BUFFER_POOL_CAST (object);
  priv = pool->priv;

  GST_DEBUG_OBJECT (pool, "%p dispose", pool);

  gst_buffer_pool_set_active (pool, FALSE);
  gst_clear_object (&priv->allocator);

  G_OBJECT_CLASS (gst_buffer_pool_parent_class)->dispose (object);
}

static void
gst_buffer_pool_finalize (GObject * object)
{
  GstBufferPool *pool;
  GstBufferPoolPrivate *priv;

  pool = GST_BUFFER_POOL_CAST (object);
  priv = pool->priv;

  GST_DEBUG_OBJECT (pool, "%p finalize", pool);

  gst_atomic_queue_unref (priv->queue);
  gst_poll_free (priv->poll);
  gst_structure_free (priv->config);
  g_rec_mutex_clear (&priv->rec_lock);

  G_OBJECT_CLASS (gst_buffer_pool_parent_class)->finalize (object);
}


/***
 * @name: gst_buffer_pool_new
 * @brief: 创建一个 #GstBufferPool实例
 * @note: 该函数会调用 gst_object_ref_sink 清除浮点引用标志
*/
GstBufferPool *
gst_buffer_pool_new (void)
{
  GstBufferPool *result;

  result = g_object_new (GST_TYPE_BUFFER_POOL, NULL);
  GST_DEBUG_OBJECT (result, "created new buffer pool");

  /* Clear floating flag */
  gst_object_ref_sink (result);

  return result;
}

static GstFlowReturn
default_alloc_buffer (GstBufferPool * pool, GstBuffer ** buffer,
    GstBufferPoolAcquireParams * params)
{
  GstBufferPoolPrivate *priv = pool->priv;

  *buffer =
      gst_buffer_new_allocate (priv->allocator, priv->size, &priv->params);

  if (!*buffer)
    return GST_FLOW_ERROR;

  return GST_FLOW_OK;
}

static gboolean
mark_meta_pooled (GstBuffer * buffer, GstMeta ** meta, gpointer user_data)
{
  GST_DEBUG_OBJECT (GST_BUFFER_POOL (user_data),
      "marking meta %p as POOLED in buffer %p", *meta, buffer);
  GST_META_FLAG_SET (*meta, GST_META_FLAG_POOLED);
  GST_META_FLAG_SET (*meta, GST_META_FLAG_LOCKED);

  return TRUE;
}

/**
 * @name: do_alloc_buffer
 * @calledby: default_acquire_buffer ()
 *            default_start ()
*/
static GstFlowReturn
do_alloc_buffer (GstBufferPool * pool, GstBuffer ** buffer,
    GstBufferPoolAcquireParams * params)
{
  GstBufferPoolPrivate *priv = pool->priv;
  GstFlowReturn result;
  gint cur_buffers, max_buffers;
  GstBufferPoolClass *pclass;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  if (G_UNLIKELY (!pclass->alloc_buffer))
    goto no_function;

  max_buffers = priv->max_buffers;

  /* increment the allocation counter */
  cur_buffers = g_atomic_int_add (&priv->cur_buffers, 1);
  if (max_buffers && cur_buffers >= max_buffers)
    goto max_reached;

  result = pclass->alloc_buffer (pool, buffer, params);
  if (G_UNLIKELY (result != GST_FLOW_OK))
    goto alloc_failed;

  /* lock all metadata and mark as pooled, we want this to remain on
   * the buffer and we want to remove any other metadata that gets added
   * later */
  gst_buffer_foreach_meta (*buffer, mark_meta_pooled, pool);

  /* un-tag memory, this is how we expect the buffer when it is
   * released again */
  GST_BUFFER_FLAG_UNSET (*buffer, GST_BUFFER_FLAG_TAG_MEMORY);

  GST_LOG_OBJECT (pool, "allocated buffer %d/%d, %p", cur_buffers,
      max_buffers, *buffer);

  return result;

  /* ERRORS */
no_function:
  {
    GST_ERROR_OBJECT (pool, "no alloc function");
    return GST_FLOW_NOT_SUPPORTED;
  }
max_reached:
  {
    GST_DEBUG_OBJECT (pool, "max buffers reached");
    g_atomic_int_add (&priv->cur_buffers, -1);
    return GST_FLOW_EOS;
  }
alloc_failed:
  {
    GST_WARNING_OBJECT (pool, "alloc function failed");
    g_atomic_int_add (&priv->cur_buffers, -1);
    return result;
  }
}


/**
 * @name: default_start
 * @brief: 启动缓冲池。默认实现将进行预分配min-buffers缓冲区并将它们放入队列
 * @calledby: do_start ()
 * @note: 这是GstBufferPoolClass->start虚函数的默认实现
*/
static gboolean
default_start (GstBufferPool * pool)
{
  guint i;
  GstBufferPoolPrivate *priv = pool->priv;
  GstBufferPoolClass *pclass;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  /* 根据最小min_buffers，预分配buffers */
  for (i = 0; i < priv->min_buffers; i++) {
    GstBuffer *buffer;

    if (do_alloc_buffer (pool, &buffer, NULL) != GST_FLOW_OK)
      goto alloc_failed;

    /* 把buffer放入到队列里面 */
    if (G_LIKELY (pclass->release_buffer))
      pclass->release_buffer (pool, buffer);
  }
  return TRUE;

  /* ERRORS */
alloc_failed:
  {
    GST_WARNING_OBJECT (pool, "failed to allocate buffer");
    return FALSE;
  }
}


/**
 * @name: do_start
 * @brief: 启动缓冲池。默认实现将进行预分配min-buffers缓冲区并将它们放入队列
 * @calledby: gst_buffer_pool_set_active ()
 * @note: 被调用的时候一定要用锁
*/
static gboolean
do_start (GstBufferPool * pool)
{
  GstBufferPoolPrivate *priv = pool->priv;

  if (!priv->started) {
    GstBufferPoolClass *pclass;

    pclass = GST_BUFFER_POOL_GET_CLASS (pool);

    GST_LOG_OBJECT (pool, "starting");
    /* pclass->start (pool)应该启动pool，应该分配buffers，放buffers到队列 */
    if (G_LIKELY (pclass->start)) {
      if (!pclass->start (pool))
        return FALSE;
    }
    priv->started = TRUE;
  }
  return TRUE;
}

static void
default_free_buffer (GstBufferPool * pool, GstBuffer * buffer)
{
  gst_buffer_unref (buffer);
}

static void
do_free_buffer (GstBufferPool * pool, GstBuffer * buffer)
{
  GstBufferPoolPrivate *priv;
  GstBufferPoolClass *pclass;

  priv = pool->priv;
  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  g_atomic_int_add (&priv->cur_buffers, -1);
  GST_LOG_OBJECT (pool, "freeing buffer %p (%u left)", buffer,
      priv->cur_buffers);

  if (G_LIKELY (pclass->free_buffer))
    pclass->free_buffer (pool, buffer);
}

/**
 * @name: default_stop
 * @calledby: do_stop会调用该虚函数实现
 * @note: 1. 这是GstBufferPoolClass->default_stop虚函数的默认实现
 *        2. 一定要被锁，保证多线程安全
*/
static gboolean
default_stop (GstBufferPool * pool)
{
  GstBufferPoolPrivate *priv = pool->priv;
  GstBuffer *buffer;

  /* clear the pool */
  while ((buffer = gst_atomic_queue_pop (priv->queue))) {
    while (!gst_poll_read_control (priv->poll)) {
      if (errno == EWOULDBLOCK) {
        /* We put the buffer into the queue but did not finish writing control
         * yet, let's wait a bit and retry */
        g_thread_yield ();
        continue;
      } else {
        /* Critical error but GstPoll already complained */
        break;
      }
    }
    do_free_buffer (pool, buffer);
  }
  return priv->cur_buffers == 0;
}


/**
 * @name: do_stop
 * @brief: 调用stop虚函数，开始flag设定FALSE
 * @call: pclass->stop (pool)，如果是默认的stop就是 default_stop ()
 * @calledby: gst_buffer_pool_set_active ()
 *            dec_outstanding ()
 * @note: must be called with the lock
*/
static gboolean
do_stop (GstBufferPool * pool)
{
  GstBufferPoolPrivate *priv = pool->priv;

  if (priv->started) {
    GstBufferPoolClass *pclass;

    pclass = GST_BUFFER_POOL_GET_CLASS (pool);

    GST_LOG_OBJECT (pool, "stopping");
    if (G_LIKELY (pclass->stop)) {
      if (!pclass->stop (pool))
        return FALSE;
    }
    priv->started = FALSE;
  }
  return TRUE;
}

/* must be called with the lock */
static void
do_set_flushing (GstBufferPool * pool, gboolean flushing)
{
  GstBufferPoolPrivate *priv = pool->priv;
  GstBufferPoolClass *pclass;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  if (GST_BUFFER_POOL_IS_FLUSHING (pool) == flushing)
    return;

  if (flushing) {
    g_atomic_int_set (&pool->flushing, 1);
    /* Write the flush token to wake up any waiters */
    gst_poll_write_control (priv->poll);

    if (pclass->flush_start)
      pclass->flush_start (pool);
  } else {
    if (pclass->flush_stop)
      pclass->flush_stop (pool);

    while (!gst_poll_read_control (priv->poll)) {
      if (errno == EWOULDBLOCK) {
        /* This should not really happen unless flushing and unflushing
         * happens on different threads. Let's wait a bit to get back flush
         * token from the thread that was setting it to flushing */
        g_thread_yield ();
        continue;
      } else {
        /* Critical error but GstPoll already complained */
        break;
      }
    }

    g_atomic_int_set (&pool->flushing, 0);
  }
}

/**
 * gst_buffer_pool_set_active:
 * @pool: a #GstBufferPool
 * @active: the new active state
 *
 * Controls the active state of @pool. When the pool is inactive, new calls to
 * gst_buffer_pool_acquire_buffer() will return with %GST_FLOW_FLUSHING.
 *
 * Activating the bufferpool will preallocate all resources in the pool based on
 * the configuration of the pool.
 *
 * Deactivating will free the resources again when there are no outstanding
 * buffers. When there are outstanding buffers, they will be freed as soon as
 * they are all returned to the pool.
 *
 * Returns: %FALSE when the pool was not configured or when preallocation of the
 * buffers failed.
 */
/**
 * @name: gst_buffer_pool_set_active
 * @breif: 控制@pool的激活状态
 *  
 * 当@pool处于非活动状态时，
*/
gboolean
gst_buffer_pool_set_active (GstBufferPool * pool, gboolean active)
{
  gboolean res = TRUE;
  GstBufferPoolPrivate *priv;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), FALSE);

  GST_LOG_OBJECT (pool, "active %d", active);

  priv = pool->priv;

  GST_BUFFER_POOL_LOCK (pool);
  /* 不需要改变状态 */
  if (priv->active == active)
    goto was_ok;

  /* 是否已经配置成功 */
  if (!priv->configured)
    goto not_configured;

  if (active) {  /* 设定到激活状态 */
    if (!do_start (pool))
      goto start_failed;

    /* flush_stop my release buffers, setting to active to avoid running
     * do_stop while activating the pool */
    priv->active = TRUE;

    /* unset the flushing state now */
    do_set_flushing (pool, FALSE);
  } else { /* 设定到未激活状态 */
    gint outstanding;

    /* 设定flushing为TRUE */
    do_set_flushing (pool, TRUE);

    /* when all buffers are in the pool, free them. Else they will be
     * freed when they are released */
    outstanding = g_atomic_int_get (&priv->outstanding);
    GST_LOG_OBJECT (pool, "outstanding buffers %d", outstanding);
    if (outstanding == 0) {
      if (!do_stop (pool))
        goto stop_failed;
    }

    priv->active = FALSE;
  }
  GST_BUFFER_POOL_UNLOCK (pool);

  return res;

was_ok:
  {
    GST_DEBUG_OBJECT (pool, "pool was in the right state");
    GST_BUFFER_POOL_UNLOCK (pool);
    return TRUE;
  }
not_configured:
  {
    GST_ERROR_OBJECT (pool, "pool was not configured");
    GST_BUFFER_POOL_UNLOCK (pool);
    return FALSE;
  }
start_failed:
  {
    GST_ERROR_OBJECT (pool, "start failed");
    GST_BUFFER_POOL_UNLOCK (pool);
    return FALSE;
  }
stop_failed:
  {
    GST_WARNING_OBJECT (pool, "stop failed");
    GST_BUFFER_POOL_UNLOCK (pool);
    return FALSE;
  }
}

/**
 * gst_buffer_pool_is_active:
 * @pool: a #GstBufferPool
 *
 * Checks if @pool is active. A pool can be activated with the
 * gst_buffer_pool_set_active() call.
 *
 * Returns: %TRUE when the pool is active.
 */
gboolean
gst_buffer_pool_is_active (GstBufferPool * pool)
{
  gboolean res;

  GST_BUFFER_POOL_LOCK (pool);
  res = pool->priv->active;
  GST_BUFFER_POOL_UNLOCK (pool);

  return res;
}

/**
 * @name: default_set_config
 * @calledby: gst_buffer_pool_set_config会调用该虚函数实现
 * @note: 这是GstBufferPoolClass->set_config虚函数的默认实现
*/
static gboolean
default_set_config (GstBufferPool * pool, GstStructure * config)
{
  GstBufferPoolPrivate *priv = pool->priv;
  GstCaps *caps;
  guint size, min_buffers, max_buffers;
  GstAllocator *allocator;
  GstAllocationParams params;

  /* parse the config and keep around */
  if (!gst_buffer_pool_config_get_params (config, &caps, &size, &min_buffers,
          &max_buffers))
    goto wrong_config;

  if (!gst_buffer_pool_config_get_allocator (config, &allocator, &params))
    goto wrong_config;

  GST_DEBUG_OBJECT (pool, "config %" GST_PTR_FORMAT, config);

  priv->size = size;
  priv->min_buffers = min_buffers;
  priv->max_buffers = max_buffers;
  priv->cur_buffers = 0;

  if (priv->allocator)
    gst_object_unref (priv->allocator);
  if ((priv->allocator = allocator))
    gst_object_ref (allocator);
  priv->params = params;

  return TRUE;

wrong_config:
  {
    GST_WARNING_OBJECT (pool, "invalid config %" GST_PTR_FORMAT, config);
    return FALSE;
  }
}


/**
 * @name: gst_buffer_pool_set_config
 * @brief: 设定pool的配置
 *        
 * 如果pool早已被配置，但是配置没有被改变，这个函数将会返回TRUE。
 * 如果pool是激活的，这个函数将返回FALSE，然后有效的配置不会被改变。
 * 从这个pool分配的buffers必须被归还，否则这个函数将不会执行任何操作并返回FALSE。
 * 
 * @config 是一个 #GstStructrue，包含了pool的配置参数，可以使用 gst_buffer_pool_config_set_params(), 
 * gst_buffer_pool_config_set_allocator() 和 gst_buffer_pool_config_add_option() 设定一组默认且必须得参数
 * 
 * 如果 @config 不是被准确设定，此函数将返回FALSE，这个函数还会更新更多的状态。可以使用 gst_buffer_pool_get_config() 检索并完善新状态。
*/
gboolean
gst_buffer_pool_set_config (GstBufferPool * pool, GstStructure * config)
{
  gboolean result;
  GstBufferPoolClass *pclass;
  GstBufferPoolPrivate *priv;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), FALSE);
  g_return_val_if_fail (config != NULL, FALSE);

  priv = pool->priv;

  GST_BUFFER_POOL_LOCK (pool);

  /* 如果config没有变化，我们就什么都不执行 */
  if (priv->configured && gst_structure_is_equal (config, priv->config))
    goto config_unchanged;

  /* 当pool处于active状态，不能修改config */
  if (priv->active)
    goto was_active;

  /* 如果有buffer没有归还回来，我们也不能改变config */
  if (g_atomic_int_get (&priv->outstanding) != 0)
    goto have_outstanding;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  /* 设置新的config*/
  if (G_LIKELY (pclass->set_config))
    result = pclass->set_config (pool, config);
  else
    result = FALSE;

  /* 保存@config到私有结构体，为了后续用户能够查看配置修改信息 
   * 有时候自定义的配置结构体，可能无法接受用户输入的配置信息，会自动更为其他信息
   */
  if (priv->config)
    gst_structure_free (priv->config);
  priv->config = config;

  if (result) {
    /* now we are configured */
    priv->configured = TRUE;
  }
  GST_BUFFER_POOL_UNLOCK (pool);

  return result;

config_unchanged:
  {
    gst_structure_free (config);
    GST_BUFFER_POOL_UNLOCK (pool);
    return TRUE;
  }
  /* ERRORS */
was_active:
  {
    gst_structure_free (config);
    GST_INFO_OBJECT (pool, "can't change config, we are active");
    GST_BUFFER_POOL_UNLOCK (pool);
    return FALSE;
  }
have_outstanding:
  {
    gst_structure_free (config);
    GST_WARNING_OBJECT (pool, "can't change config, have outstanding buffers");
    GST_BUFFER_POOL_UNLOCK (pool);
    return FALSE;
  }
}


/**
 * @name: gst_buffer_pool_get_config
 * @brief: 获取bufferpool配置（这个配置是被拷贝了一份返回给我们）
*/
GstStructure *
gst_buffer_pool_get_config (GstBufferPool * pool)
{
  GstStructure *result;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), NULL);

  GST_BUFFER_POOL_LOCK (pool);
  result = gst_structure_copy (pool->priv->config);
  GST_BUFFER_POOL_UNLOCK (pool);

  return result;
}

static const gchar *empty_option[] = { NULL };


/**
 * @name: gst_buffer_pool_get_options
 * @brief: 得到一个以%NULL结尾的字符串数组，这些字符串表示@pool支持的选项
 *         通常使用 gst_buffer_pool_config_add_option() 来启用一个选项
*/
const gchar **
gst_buffer_pool_get_options (GstBufferPool * pool)
{
  GstBufferPoolClass *pclass;
  const gchar **result;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), NULL);

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  if (G_LIKELY (pclass->get_options)) {
    if ((result = pclass->get_options (pool)) == NULL)
      goto invalid_result;
  } else
    result = empty_option;

  return result;

  /* ERROR */
invalid_result:
  {
    g_warning ("bufferpool subclass returned NULL options");
    return empty_option;
  }
}


/* 检查@pool是否支持该@option */
gboolean
gst_buffer_pool_has_option (GstBufferPool * pool, const gchar * option)
{
  guint i;
  const gchar **options;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), FALSE);
  g_return_val_if_fail (option != NULL, FALSE);

  options = gst_buffer_pool_get_options (pool);

  for (i = 0; options[i]; i++) {
    if (g_str_equal (options[i], option))
      return TRUE;
  }
  return FALSE;
}

/**
 * @name: gst_buffer_pool_config_set_params
 * @param config: a #GstBufferPool configuration
 * @param caps: (nullable): caps for the buffers
 * @param size: the size of each buffer, not including prefix and padding
 * @param min_buffers: 分配buffers的最小数量
 * @param max_buffers: 分配buffers的最大数量，0表示最大数量没有限制
 * @brief: 使用被给参数设定@config结构体
*/
void
gst_buffer_pool_config_set_params (GstStructure * config, GstCaps * caps,
    guint size, guint min_buffers, guint max_buffers)
{
  g_return_if_fail (config != NULL);
  g_return_if_fail (max_buffers == 0 || min_buffers <= max_buffers);
  g_return_if_fail (caps == NULL || gst_caps_is_fixed (caps));

  gst_structure_id_set (config,
      GST_QUARK (CAPS), GST_TYPE_CAPS, caps,
      GST_QUARK (SIZE), G_TYPE_UINT, size,
      GST_QUARK (MIN_BUFFERS), G_TYPE_UINT, min_buffers,
      GST_QUARK (MAX_BUFFERS), G_TYPE_UINT, max_buffers, NULL);
}


/**
 * @name: gst_buffer_pool_config_set_allocator
 * @brief: 使用 @allocator 和 @params 设定 @config结构体
 *       
 * @allocator和@params能够有一个设定为NULL（两者不能同时都为NULL）
 * @allocator = NULL 使用默认的分配器，@params = NULL 默认的 #GstAllocationParams
 * 
 * 调用 gst_buffer_pool_set_config() 来更新能够执行的分配器和params值。
 * 例如：有些pool无法使用不同的分配器，或不能使用在@params中指定的值进行分配。
 *      可以使用 gst_buffer_pool_get_config() 来获取当前使用的值。
*/
void
gst_buffer_pool_config_set_allocator (GstStructure * config,
    GstAllocator * allocator, const GstAllocationParams * params)
{
  g_return_if_fail (config != NULL);
  g_return_if_fail (allocator != NULL || params != NULL);

  gst_structure_id_set (config,
      GST_QUARK (ALLOCATOR), GST_TYPE_ALLOCATOR, allocator,
      GST_QUARK (PARAMS), GST_TYPE_ALLOCATION_PARAMS, params, NULL);
}


/**
 * @name: gst_buffer_pool_config_add_option
 * @brief: 如果存储选项的 GST_TYPE_ARRAY 还没有创建，会先创建GST_QUARK (OPTIONS)对应的GValue
 * 使能在@config中的选项。这将指示@bufferpool在其分配的缓冲区上启用指定的选项。
 *         可以使用  gst_buffer_pool_get_options() 检索 @pool 支持的选项
*/
void
gst_buffer_pool_config_add_option (GstStructure * config, const gchar * option)
{
  const GValue *value;
  GValue option_value = { 0, };
  guint i, len;

  g_return_if_fail (config != NULL);

  /* 从@config中获取选项key对象的GValue，注意这个GValue的类型是GST_TYPE_ARRAY */
  value = gst_structure_id_get_value (config, GST_QUARK (OPTIONS));

  if (value) { /* 如果存在该GValue*/
    len = gst_value_array_get_size (value);
    for (i = 0; i < len; ++i) {
      const GValue *nth_val = gst_value_array_get_value (value, i);

      if (g_str_equal (option, g_value_get_string (nth_val))) /*  */
        return;
    }
  } else { /* 如果不存储，则创建 */
    GValue new_array_val = { 0, };

    g_value_init (&new_array_val, GST_TYPE_ARRAY);
    /* 用 GST_QUARK (OPTIONS) 和 new_array_val 设定 config */
    gst_structure_id_take_value (config, GST_QUARK (OPTIONS), &new_array_val);
    value = gst_structure_id_get_value (config, GST_QUARK (OPTIONS));
  }
  /* 将@option添加到选项key对应的GST_TYPE_ARRAY  */
  g_value_init (&option_value, G_TYPE_STRING);
  g_value_set_string (&option_value, option);
  gst_value_array_append_and_take_value ((GValue *) value, &option_value);
}


/* 查看当前@config的options存储了多少个option */
guint
gst_buffer_pool_config_n_options (GstStructure * config)
{
  const GValue *value;
  guint size = 0;

  g_return_val_if_fail (config != NULL, 0);

  value = gst_structure_id_get_value (config, GST_QUARK (OPTIONS));
  if (value) {
    size = gst_value_array_get_size (value);
  }
  return size;
}


/* 获取@index索引对应的选项 */
const gchar *
gst_buffer_pool_config_get_option (GstStructure * config, guint index)
{
  const GValue *value;
  const gchar *ret = NULL;

  g_return_val_if_fail (config != NULL, 0);

  value = gst_structure_id_get_value (config, GST_QUARK (OPTIONS));
  if (value) {
    const GValue *option_value;

    option_value = gst_value_array_get_value (value, index);
    if (option_value)
      ret = g_value_get_string (option_value);
  }
  return ret;
}


/* 检查@config是否有@option */
gboolean
gst_buffer_pool_config_has_option (GstStructure * config, const gchar * option)
{
  const GValue *value;
  guint i, len;

  g_return_val_if_fail (config != NULL, 0);

  value = gst_structure_id_get_value (config, GST_QUARK (OPTIONS));
  if (value) {
    len = gst_value_array_get_size (value);
    for (i = 0; i < len; ++i) {
      const GValue *nth_val = gst_value_array_get_value (value, i);

      if (g_str_equal (option, g_value_get_string (nth_val)))
        return TRUE;
    }
  }
  return FALSE;
}


/**
 * @name: gst_buffer_pool_config_get_params
 * @param config: (transfer none): a #GstBufferPool configuration
 * @param caps: (out) (transfer none) (optional) (nullable): the caps of buffers
 * @param size: (out) (optional): the size of each buffer, not including prefix and padding
 * @param min_buffers: (out) (optional): the minimum amount of buffers to allocate.
 * @param max_buffers: (out) (optional): the maximum amount of buffers to allocate or 0 for unlimited.
 * @breif: 从@config获取到输出参数对应的值
*/
gboolean
gst_buffer_pool_config_get_params (GstStructure * config, GstCaps ** caps,
    guint * size, guint * min_buffers, guint * max_buffers)
{
  g_return_val_if_fail (config != NULL, FALSE);

  if (caps) {
    *caps = g_value_get_boxed (gst_structure_id_get_value (config,
            GST_QUARK (CAPS)));
  }
  return gst_structure_id_get (config,
      GST_QUARK (SIZE), G_TYPE_UINT, size,
      GST_QUARK (MIN_BUFFERS), G_TYPE_UINT, min_buffers,
      GST_QUARK (MAX_BUFFERS), G_TYPE_UINT, max_buffers, NULL);
}


/**
 * @name: gst_buffer_pool_config_get_allocator
 * @param config: (transfer none): a #GstBufferPool configuration
 * @param allocator: (out) (optional) (nullable) (transfer none): a #GstAllocator, or %NULL
 * @param params: (out caller-allocates) (optional): #GstAllocationParams, or %NULL
 * @brief: 从@config得到@allocator和@params
*/
gboolean
gst_buffer_pool_config_get_allocator (GstStructure * config,
    GstAllocator ** allocator, GstAllocationParams * params)
{
  g_return_val_if_fail (config != NULL, FALSE);

  if (allocator)
    *allocator = g_value_get_object (gst_structure_id_get_value (config,
            GST_QUARK (ALLOCATOR)));
  if (params) {
    GstAllocationParams *p;

    p = g_value_get_boxed (gst_structure_id_get_value (config,
            GST_QUARK (PARAMS)));
    if (p) {
      *params = *p;
    } else {
      gst_allocation_params_init (params);
    }
  }
  return TRUE;
}


/**
 * @name gst_buffer_pool_config_validate_params:
 * @param config: (transfer none): 一个 #GstBufferPool 配置
 * @param caps: (nullable) (transfer none): 缓冲区预期的 caps
 * @param size: 每个缓冲区的预期大小，不包括前缀和填充
 * @param min_buffers: 预期分配的最小缓冲区数量
 * @param max_buffers: 预期分配的最大缓冲区数量，或者为 0 表示无限制
 *
 * 验证对 @config 所做的更改在预期参数的上下文中仍然有效。这个函数是一个辅助工具，可以用来验证当 gst_buffer_pool_set_config()
 * 返回 %FALSE 时，池对配置所做的更改。这期望 @caps 没有变化，并且 @min_buffers 不低于我们最初的预期。
 * 这不会检查选项或分配器参数是否仍然有效，也不会检查大小是否有变化，因为改变大小以适应填充是有效的。
 *
 * 自 1.4 版本以来
 *
 * 返回: 如果参数在这个上下文中有效，则返回 %TRUE。
 */
gboolean
gst_buffer_pool_config_validate_params (GstStructure * config, GstCaps * caps,
    guint size, guint min_buffers, G_GNUC_UNUSED guint max_buffers)
{
  GstCaps *newcaps;
  guint newsize, newmin;
  gboolean ret = FALSE;

  g_return_val_if_fail (config != NULL, FALSE);

  gst_buffer_pool_config_get_params (config, &newcaps, &newsize, &newmin, NULL);

  if (gst_caps_is_equal (caps, newcaps) && (newsize >= size)
      && (newmin >= min_buffers))
    ret = TRUE;

  return ret;
}

static GstFlowReturn
default_acquire_buffer (GstBufferPool * pool, GstBuffer ** buffer,
    GstBufferPoolAcquireParams * params)
{
  GstFlowReturn result;
  GstBufferPoolPrivate *priv = pool->priv;

  while (TRUE) {
    if (G_UNLIKELY (GST_BUFFER_POOL_IS_FLUSHING (pool)))
      goto flushing;

    /* try to get a buffer from the queue */
    *buffer = gst_atomic_queue_pop (priv->queue);
    if (G_LIKELY (*buffer)) {
      while (!gst_poll_read_control (priv->poll)) {
        if (errno == EWOULDBLOCK) {
          /* We put the buffer into the queue but did not finish writing control
           * yet, let's wait a bit and retry */
          g_thread_yield ();
          continue;
        } else {
          /* Critical error but GstPoll already complained */
          break;
        }
      }
      result = GST_FLOW_OK;
      GST_LOG_OBJECT (pool, "acquired buffer %p", *buffer);
      break;
    }

    /* no buffer, try to allocate some more */
    GST_LOG_OBJECT (pool, "no buffer, trying to allocate");
    result = do_alloc_buffer (pool, buffer, params);
    if (G_LIKELY (result == GST_FLOW_OK))
      /* we have a buffer, return it */
      break;

    if (G_UNLIKELY (result != GST_FLOW_EOS))
      /* something went wrong, return error */
      break;

    /* check if we need to wait */
    if (params && (params->flags & GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT)) {
      GST_LOG_OBJECT (pool, "no more buffers");
      break;
    }

    /* now we release the control socket, we wait for a buffer release or
     * flushing */
    if (!gst_poll_read_control (pool->priv->poll)) {
      if (errno == EWOULDBLOCK) {
        /* This means that we have two threads trying to allocate buffers
         * already, and the other one already got the wait token. This
         * means that we only have to wait for the poll now and not write the
         * token afterwards: we will be woken up once the other thread is
         * woken up and that one will write the wait token it removed */
        GST_LOG_OBJECT (pool, "waiting for free buffers or flushing");
        gst_poll_wait (priv->poll, GST_CLOCK_TIME_NONE);
      } else {
        /* This is a critical error, GstPoll already gave a warning */
        result = GST_FLOW_ERROR;
        break;
      }
    } else {
      /* We're the first thread waiting, we got the wait token and have to
       * write it again later
       * OR
       * We're a second thread and just consumed the flush token and block all
       * other threads, in which case we must not wait and give it back
       * immediately */
      if (!GST_BUFFER_POOL_IS_FLUSHING (pool)) {
        GST_LOG_OBJECT (pool, "waiting for free buffers or flushing");
        gst_poll_wait (priv->poll, GST_CLOCK_TIME_NONE);
      }
      gst_poll_write_control (pool->priv->poll);
    }
  }

  return result;

  /* ERRORS */
flushing:
  {
    GST_DEBUG_OBJECT (pool, "we are flushing");
    return GST_FLOW_FLUSHING;
  }
}

static inline void
dec_outstanding (GstBufferPool * pool)
{
  if (g_atomic_int_dec_and_test (&pool->priv->outstanding)) {
    /* all buffers are returned to the pool, see if we need to free them */
    if (GST_BUFFER_POOL_IS_FLUSHING (pool)) {
      /* take the lock so that set_active is not run concurrently */
      GST_BUFFER_POOL_LOCK (pool);
      /* now that we have the lock, check if we have been de-activated with
       * outstanding buffers */
      if (!pool->priv->active)
        do_stop (pool);

      GST_BUFFER_POOL_UNLOCK (pool);
    }
  }
}

static gboolean
remove_meta_unpooled (GstBuffer * buffer, GstMeta ** meta, gpointer user_data)
{
  if (!GST_META_FLAG_IS_SET (*meta, GST_META_FLAG_POOLED)) {
    GST_META_FLAG_UNSET (*meta, GST_META_FLAG_LOCKED);
    *meta = NULL;
  }
  return TRUE;
}

static void
default_reset_buffer (GstBufferPool * pool, GstBuffer * buffer)
{
  GST_BUFFER_FLAGS (buffer) &= GST_BUFFER_FLAG_TAG_MEMORY;

  GST_BUFFER_PTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DURATION (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_OFFSET (buffer) = GST_BUFFER_OFFSET_NONE;
  GST_BUFFER_OFFSET_END (buffer) = GST_BUFFER_OFFSET_NONE;

  /* if the memory is intact reset the size to the full size */
  if (!GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY)) {
    gsize offset, maxsize;
    gst_buffer_get_sizes (buffer, &offset, &maxsize);
    /* check if we can resize to at least the pool configured size.  If not,
     * then this will fail internally in gst_buffer_resize().
     * default_release_buffer() will drop the buffer from the pool if the
     * sizes don't match */
    if (maxsize >= pool->priv->size) {
      gst_buffer_resize (buffer, -offset, pool->priv->size);
    } else {
      GST_WARNING_OBJECT (pool, "Buffer %p without the memory tag has "
          "maxsize (%" G_GSIZE_FORMAT ") that is smaller than the "
          "configured buffer pool size (%u). The buffer will be not be "
          "reused. This is most likely a bug in this GstBufferPool subclass",
          buffer, maxsize, pool->priv->size);
    }
  }

  /* remove all metadata without the POOLED flag */
  gst_buffer_foreach_meta (buffer, remove_meta_unpooled, pool);
}

/**
 * gst_buffer_pool_acquire_buffer:
 * @pool: a #GstBufferPool
 * @buffer: (out) (transfer full) (nullable): a location for a #GstBuffer
 * @params: (transfer none) (nullable): parameters.
 *
 * Acquires a buffer from @pool. @buffer should point to a memory location that
 * can hold a pointer to the new buffer. When the pool is empty, this function
 * will by default block until a buffer is released into the pool again or when
 * the pool is set to flushing or deactivated.
 *
 * @params can contain optional parameters to influence the allocation.
 *
 * Returns: a #GstFlowReturn such as %GST_FLOW_FLUSHING when the pool is
 * inactive.
 */
GstFlowReturn
gst_buffer_pool_acquire_buffer (GstBufferPool * pool, GstBuffer ** buffer,
    GstBufferPoolAcquireParams * params)
{
  GstBufferPoolClass *pclass;
  GstFlowReturn result;

  g_return_val_if_fail (GST_IS_BUFFER_POOL (pool), GST_FLOW_ERROR);
  g_return_val_if_fail (buffer != NULL, GST_FLOW_ERROR);

  *buffer = NULL;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  /* assume we'll have one more outstanding buffer we need to do that so
   * that concurrent set_active doesn't clear the buffers */
  g_atomic_int_inc (&pool->priv->outstanding);

  if (G_LIKELY (pclass->acquire_buffer))
    result = pclass->acquire_buffer (pool, buffer, params);
  else
    result = GST_FLOW_NOT_SUPPORTED;

  if (G_LIKELY (result == GST_FLOW_OK)) {
    /* all buffers from the pool point to the pool and have the refcount of the
     * pool incremented */
    (*buffer)->pool = gst_object_ref (pool);
  } else {
    dec_outstanding (pool);
  }

  return result;
}


/**
 * @name: default_release_buffer
 * @brief: 把@buffer放回到queue
 * @calledby: default_start ()
 *            gst_buffer_pool_release_buffer ()
 * @note: 这是GstBufferPoolClass->release_buffer = default_release_buffer虚函数的默认实现
*/
static void
default_release_buffer (GstBufferPool * pool, GstBuffer * buffer)
{
  GST_LOG_OBJECT (pool, "released buffer %p %d", buffer,
      GST_MINI_OBJECT_FLAGS (buffer));

  /* 内存块应该没有被标记 GST_BUFFER_FLAG_TAG_MEMORY */
  if (G_UNLIKELY (GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY)))
    goto memory_tagged;

  /* @buffer的size和@pool存储的size要相同 */
  if (G_UNLIKELY (gst_buffer_get_size (buffer) != pool->priv->size))
    goto size_changed;

  /* @buffeer所有内存应该是可写的 */
  if (G_UNLIKELY (!gst_buffer_is_all_memory_writable (buffer)))
    goto not_writable;

  /* 把@buffer放入队列 */
  gst_atomic_queue_push (pool->priv->queue, buffer);
  gst_poll_write_control (pool->priv->poll);

  return;

memory_tagged:
  {
    GST_CAT_DEBUG_OBJECT (GST_CAT_PERFORMANCE, pool,
        "discarding buffer %p: memory tag set", buffer);
    goto discard;
  }
size_changed:
  {
    GST_CAT_DEBUG_OBJECT (GST_CAT_PERFORMANCE, pool,
        "discarding buffer %p: size %" G_GSIZE_FORMAT " != %u",
        buffer, gst_buffer_get_size (buffer), pool->priv->size);
    goto discard;
  }
not_writable:
  {
    GST_CAT_DEBUG_OBJECT (GST_CAT_PERFORMANCE, pool,
        "discarding buffer %p: memory not writable", buffer);
    goto discard;
  }
discard:
  {
    do_free_buffer (pool, buffer);
    gst_poll_write_control (pool->priv->poll);
    return;
  }
}


/**
 * @name: gst_buffer_pool_release_buffer
 * @brief: 将 @buffer 释放到 @pool。
 *         @buffer 应该之前已经从 gst_buffer_pool_acquire_buffer() 函数分配得到。
 * @note: 当 @buffer 上的最后一个引用消失时，这个函数通常会被自动调用。
*/
void
gst_buffer_pool_release_buffer (GstBufferPool * pool, GstBuffer * buffer)
{
  GstBufferPoolClass *pclass;

  g_return_if_fail (GST_IS_BUFFER_POOL (pool));
  g_return_if_fail (buffer != NULL);

  /* check that the buffer is ours, all buffers returned to the pool have the
   * pool member set to NULL and the pool refcount decreased */
  if (!g_atomic_pointer_compare_and_exchange (&buffer->pool, pool, NULL))
    return;

  pclass = GST_BUFFER_POOL_GET_CLASS (pool);

  /* reset the buffer when needed */
  if (G_LIKELY (pclass->reset_buffer))
    pclass->reset_buffer (pool, buffer);

  if (G_LIKELY (pclass->release_buffer))
    pclass->release_buffer (pool, buffer);

  dec_outstanding (pool);

  /* decrease the refcount that the buffer had to us */
  gst_object_unref (pool);
}

/**
 * gst_buffer_pool_set_flushing:
 * @pool: a #GstBufferPool
 * @flushing: whether to start or stop flushing
 *
 * Enables or disables the flushing state of a @pool without freeing or
 * allocating buffers.
 *
 * Since: 1.4
 */
void
gst_buffer_pool_set_flushing (GstBufferPool * pool, gboolean flushing)
{
  GstBufferPoolPrivate *priv;

  g_return_if_fail (GST_IS_BUFFER_POOL (pool));

  GST_LOG_OBJECT (pool, "flushing %d", flushing);

  priv = pool->priv;

  GST_BUFFER_POOL_LOCK (pool);

  if (!priv->active) {
    GST_WARNING_OBJECT (pool, "can't change flushing state of inactive pool");
    goto done;
  }

  do_set_flushing (pool, flushing);

done:
  GST_BUFFER_POOL_UNLOCK (pool);
}
