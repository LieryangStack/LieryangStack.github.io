#include "gst_private.h"

#include "gstinfo.h"
#include "gsttask.h"
#include "glib-compat-private.h"

#include <stdio.h>

#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#ifdef HAVE_PTHREAD_SETNAME_NP_WITHOUT_TID
#include <pthread.h>
#endif

GST_DEBUG_CATEGORY_STATIC (task_debug);
#define GST_CAT_DEFAULT (task_debug)

#define SET_TASK_STATE(t,s) (g_atomic_int_set (&GST_TASK_STATE(t), (s)))
#define GET_TASK_STATE(t)   ((GstTaskState) g_atomic_int_get (&GST_TASK_STATE(t)))

struct _GstTaskPrivate
{
  /* callbacks for managing the thread of this task */
  GstTaskThreadFunc enter_func;
  gpointer enter_user_data;
  GDestroyNotify enter_notify;

  GstTaskThreadFunc leave_func;
  gpointer leave_user_data;
  GDestroyNotify leave_notify;

  /* configured pool */
  GstTaskPool *pool;

  /* remember the pool and id that is currently running. */
  gpointer id;
  GstTaskPool *pool_id;
};

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef HRESULT (WINAPI * pSetThreadDescription) (HANDLE hThread,
    PCWSTR lpThreadDescription);
static pSetThreadDescription SetThreadDescriptionFunc = NULL;
HMODULE kernel32_module = NULL;

struct _THREADNAME_INFO
{
  DWORD dwType;                 // must be 0x1000
  LPCSTR szName;                // pointer to name (in user addr space)
  DWORD dwThreadID;             // thread ID (-1=caller thread)
  DWORD dwFlags;                // reserved for future use, must be zero
};
typedef struct _THREADNAME_INFO THREADNAME_INFO;

static void
SetThreadName (DWORD dwThreadID, LPCSTR szThreadName)
{
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = szThreadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;

  __try {
    RaiseException (0x406D1388, 0, sizeof (info) / sizeof (DWORD),
        (const ULONG_PTR *) &info);
  }
  __except (EXCEPTION_CONTINUE_EXECUTION) {
  }
}

static gboolean
gst_task_win32_load_library (void)
{
  /* FIXME: Add support for UWP app */
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  static gsize _init_once = 0;
  if (g_once_init_enter (&_init_once)) {
    kernel32_module = LoadLibraryW (L"kernel32.dll");
    if (kernel32_module) {
      SetThreadDescriptionFunc =
          (pSetThreadDescription) GetProcAddress (kernel32_module,
          "SetThreadDescription");
      if (!SetThreadDescriptionFunc)
        FreeLibrary (kernel32_module);
    }
    g_once_init_leave (&_init_once, 1);
  }
#endif

  return ! !SetThreadDescriptionFunc;
}

static gboolean
gst_task_win32_set_thread_desc (const gchar * name)
{
  HRESULT hr;
  wchar_t *namew;

  if (!gst_task_win32_load_library () || !name)
    return FALSE;

  namew = g_utf8_to_utf16 (name, -1, NULL, NULL, NULL);
  if (!namew)
    return FALSE;

  hr = SetThreadDescriptionFunc (GetCurrentThread (), namew);

  g_free (namew);
  return SUCCEEDED (hr);
}

static void
gst_task_win32_set_thread_name (const gchar * name)
{
  /* Prefer SetThreadDescription over exception based way if available,
   * since thread description set by SetThreadDescription will be preserved
   * in dump file */
  if (!gst_task_win32_set_thread_desc (name))
    SetThreadName ((DWORD) - 1, name);
}
#endif

static void gst_task_finalize (GObject * object);

static void gst_task_func (GstTask * task);

static GMutex pool_lock;

static GstTaskPool *_global_task_pool = NULL;

#define _do_init \
{ \
  GST_DEBUG_CATEGORY_INIT (task_debug, "task", 0, "Processing tasks"); \
}

G_DEFINE_TYPE_WITH_CODE (GstTask, gst_task, GST_TYPE_OBJECT,
    G_ADD_PRIVATE (GstTask) _do_init);

/* 被调用的时候一定要使用 pool_lock */
static void
ensure_klass_pool (GstTaskClass * klass)
{
  if (G_UNLIKELY (_global_task_pool == NULL)) {
    _global_task_pool = gst_task_pool_new ();
    gst_task_pool_prepare (_global_task_pool, NULL);

    /* Classes are never destroyed so this ref will never be dropped */
    GST_OBJECT_FLAG_SET (_global_task_pool, GST_OBJECT_FLAG_MAY_BE_LEAKED);
  }
  klass->pool = _global_task_pool;
}

static void
gst_task_class_init (GstTaskClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  gobject_class->finalize = gst_task_finalize;
}

static void
gst_task_init (GstTask * task)
{
  GstTaskClass *klass;

  klass = GST_TASK_GET_CLASS (task);

  task->priv = gst_task_get_instance_private (task);
  task->running = FALSE;
  task->thread = NULL;
  task->lock = NULL;
  g_cond_init (&task->cond);
  SET_TASK_STATE (task, GST_TASK_STOPPED);

  /* 使用此任务的默认类池，可以通过设定gst_task_set_pool覆盖此方法 */
  g_mutex_lock (&pool_lock);
  ensure_klass_pool (klass);
  task->priv->pool = gst_object_ref (klass->pool);
  g_mutex_unlock (&pool_lock);
}

static void
gst_task_finalize (GObject * object)
{
  GstTask *task = GST_TASK (object);
  GstTaskPrivate *priv = task->priv;

  GST_DEBUG ("task %p finalize", task);

  if (priv->enter_notify)
    priv->enter_notify (priv->enter_user_data);

  if (priv->leave_notify)
    priv->leave_notify (priv->leave_user_data);

  if (task->notify)
    task->notify (task->user_data);

  gst_object_unref (priv->pool);

  /* task thread cannot be running here since it holds a ref
   * to the task so that the finalize could not have happened */
  g_cond_clear (&task->cond);

  G_OBJECT_CLASS (gst_task_parent_class)->finalize (object);
}

/* should be called with the object LOCK */
static void
gst_task_configure_name (GstTask * task)
{
#if defined(HAVE_SYS_PRCTL_H) && defined(PR_SET_NAME)
  const gchar *name;
  gchar thread_name[17] = { 0, };

  GST_OBJECT_LOCK (task);
  name = GST_OBJECT_NAME (task);

  /* set the thread name to something easily identifiable */
  if (!snprintf (thread_name, 17, "%s", GST_STR_NULL (name))) {
    GST_DEBUG_OBJECT (task, "Could not create thread name for '%s'", name);
  } else {
    GST_DEBUG_OBJECT (task, "Setting thread name to '%s'", thread_name);
    if (prctl (PR_SET_NAME, (unsigned long int) thread_name, 0, 0, 0))
      GST_DEBUG_OBJECT (task, "Failed to set thread name");
  }
  GST_OBJECT_UNLOCK (task);
#elif defined(HAVE_PTHREAD_SETNAME_NP_WITHOUT_TID)
  const gchar *name;

  GST_OBJECT_LOCK (task);
  name = GST_OBJECT_NAME (task);

  /* set the thread name to something easily identifiable */
  GST_DEBUG_OBJECT (task, "Setting thread name to '%s'", name);
  if (pthread_setname_np (name))
    GST_DEBUG_OBJECT (task, "Failed to set thread name");

  GST_OBJECT_UNLOCK (task);
#elif defined (_MSC_VER)
  const gchar *name;
  name = GST_OBJECT_NAME (task);

  /* set the thread name to something easily identifiable */
  GST_DEBUG_OBJECT (task, "Setting thread name to '%s'", name);
  gst_task_win32_set_thread_name (name);
#endif
}

/**
 * 线程池push函数，最终执行任务函数
*/
static void
gst_task_func (GstTask * task)
{
  GRecMutex *lock;
  GThread *tself;
  GstTaskPrivate *priv;

  priv = task->priv;

  tself = g_thread_self ();

  GST_DEBUG ("Entering task %p, thread %p", task, tself);

  /* we have to grab the lock to get the mutex. We also
   * mark our state running so that nobody can mess with
   * the mutex. */
  GST_OBJECT_LOCK (task);
  if (GET_TASK_STATE (task) == GST_TASK_STOPPED)
    goto exit;
  lock = GST_TASK_GET_LOCK (task);
  if (G_UNLIKELY (lock == NULL))
    goto no_lock;
  task->thread = tself;
  GST_OBJECT_UNLOCK (task);

  /* 如果设定的在进入任务前调用回调函数 */
  if (priv->enter_func)
    priv->enter_func (task, tself, priv->enter_user_data);

  /* locking order is TASK_LOCK, LOCK */
  g_rec_mutex_lock (lock);
  /* configure the thread name now */
  gst_task_configure_name (task);

  while (G_LIKELY (GET_TASK_STATE (task) != GST_TASK_STOPPED)) {
    GST_OBJECT_LOCK (task);
    while (G_UNLIKELY (GST_TASK_STATE (task) == GST_TASK_PAUSED)) {
      g_rec_mutex_unlock (lock);

      GST_TASK_SIGNAL (task);
      GST_INFO_OBJECT (task, "Task going to paused");
      GST_TASK_WAIT (task);
      GST_INFO_OBJECT (task, "Task resume from paused");
      GST_OBJECT_UNLOCK (task);
      /* locking order.. */
      g_rec_mutex_lock (lock);
      GST_OBJECT_LOCK (task);
    }

    if (G_UNLIKELY (GET_TASK_STATE (task) == GST_TASK_STOPPED)) {
      GST_OBJECT_UNLOCK (task);
      break;
    } else {
      GST_OBJECT_UNLOCK (task);
    }

    task->func (task->user_data);
  }

  g_rec_mutex_unlock (lock);

  GST_OBJECT_LOCK (task);
  task->thread = NULL;

exit:
  if (priv->leave_func) {
    /* fire the leave_func callback when we need to. We need to do this before
     * we signal the task and with the task lock released. */
    GST_OBJECT_UNLOCK (task);
    priv->leave_func (task, tself, priv->leave_user_data);
    GST_OBJECT_LOCK (task);
  }
  /* now we allow messing with the lock again by setting the running flag to
   * %FALSE. Together with the SIGNAL this is the sign for the _join() to
   * complete.
   * Note that we still have not dropped the final ref on the task. We could
   * check here if there is a pending join() going on and drop the last ref
   * before releasing the lock as we can be sure that a ref is held by the
   * caller of the join(). */
  task->running = FALSE;
  GST_TASK_SIGNAL (task);
  GST_OBJECT_UNLOCK (task);

  GST_DEBUG ("Exit task %p, thread %p", task, g_thread_self ());

  gst_object_unref (task);
  return;

no_lock:
  {
    g_warning ("starting task without a lock");
    goto exit;
  }
}

/**
 * gst_task_cleanup_all:
 *
 * Wait for all tasks to be stopped. This is mainly used internally
 * to ensure proper cleanup of internal data structures in test suites.
 *
 * MT safe.
 */
void
gst_task_cleanup_all (void)
{
  GstTaskClass *klass;

  if ((klass = g_type_class_peek (GST_TYPE_TASK))) {
    if (klass->pool) {
      g_mutex_lock (&pool_lock);
      gst_task_pool_cleanup (klass->pool);
      gst_object_unref (klass->pool);
      klass->pool = NULL;
      _global_task_pool = NULL;
      g_mutex_unlock (&pool_lock);
    }
  }

  /* GstElement owns a GThreadPool */
  _priv_gst_element_cleanup ();
}


/**
 * @name: gst_task_new
 * @param func: 要使用的 #GstTaskFunction
 * @param user_data: 传递给 @func 的用户数据
 * @param notify: 当@user_data不再需要的时候被自动调用函数
 * @brief: 创建一个新的任务，该任务将重复调用提供的 @func，并将 @user_data 作为参数。通常任务会在一个新线程中运行。
 *         任务创建后无法更改函数。要更改函数，必须创建一个新的 #GstTask。
 *         此函数还不会创建和启动线程。使用 gst_task_start() 或 gst_task_pause() 来创建和启动 GThread。
 *         在任务可以使用之前，必须使用 gst_task_set_lock() 函数配置一个 #GRecMutex。该锁将在调用 @func 时始终被获取。
*/
GstTask *
gst_task_new (GstTaskFunction func, gpointer user_data, GDestroyNotify notify)
{
  GstTask *task;

  g_return_val_if_fail (func != NULL, NULL);

  task = g_object_new (GST_TYPE_TASK, NULL);
  task->func = func;
  task->user_data = user_data;
  task->notify = notify;

  GST_DEBUG ("Created task %p", task);

  /* clear floating flag */
  gst_object_ref_sink (task);

  return task;
}


/**
 * @name: gst_task_set_lock
 * @param task: 要使用的 #GstTask
 * @param mutex: 要使用的 #GRecMutex
 * @brief: 设置任务使用的互斥锁。在调用 #GstTaskFunction 之前将获取该互斥锁。
 * @note: 在调用 gst_task_pause() 或 gst_task_start() 之前必须调用此函数。
 * 
 * MT safe
*/
void
gst_task_set_lock (GstTask * task, GRecMutex * mutex)
{
  g_return_if_fail (GST_IS_TASK (task));

  GST_OBJECT_LOCK (task);
  if (G_UNLIKELY (task->running))
    goto is_running;
  GST_INFO ("setting stream lock %p on task %p", mutex, task);
  GST_TASK_GET_LOCK (task) = mutex;
  GST_OBJECT_UNLOCK (task);

  return;

  /* ERRORS */
is_running:
  {
    GST_OBJECT_UNLOCK (task);
    g_warning ("cannot call set_lock on a running task");
  }
}


/* 获取@task中的线程池 */
GstTaskPool *
gst_task_get_pool (GstTask * task)
{
  GstTaskPool *result;
  GstTaskPrivate *priv;

  g_return_val_if_fail (GST_IS_TASK (task), NULL);

  priv = task->priv;

  GST_OBJECT_LOCK (task);
  result = gst_object_ref (priv->pool);
  GST_OBJECT_UNLOCK (task);

  return result;
}


/* 将 @pool 设置为 @task 的新 GstTaskPool。@task 将来创建的任何新的streaming thread将使用 @pool。 */
void
gst_task_set_pool (GstTask * task, GstTaskPool * pool)
{
  GstTaskPool *old;
  GstTaskPrivate *priv;

  g_return_if_fail (GST_IS_TASK (task));
  g_return_if_fail (GST_IS_TASK_POOL (pool));

  priv = task->priv;

  GST_OBJECT_LOCK (task);
  if (priv->pool != pool) {
    old = priv->pool;
    priv->pool = gst_object_ref (pool);
  } else
    old = NULL;
  GST_OBJECT_UNLOCK (task);

  if (old)
    gst_object_unref (old);
}

/**
 * @name: gst_task_set_enter_callback:
 * @task: 要使用的 #GstTask
 * @enter_func: （in）：一个 #GstTaskThreadFunc
 * @user_data: 传递给 @enter_func 的用户数据
 * @notify: 在 @user_data 不再被引用时调用
 *
 * 在进入 @task 的任务函数时调用 @enter_func。@user_data 将被传递给 @enter_func，并且在 @user_data 不再被引用时将调用 @notify。
 */
void
gst_task_set_enter_callback (GstTask * task, GstTaskThreadFunc enter_func,
    gpointer user_data, GDestroyNotify notify)
{
  GDestroyNotify old_notify;

  g_return_if_fail (task != NULL);
  g_return_if_fail (GST_IS_TASK (task));

  GST_OBJECT_LOCK (task);
  if ((old_notify = task->priv->enter_notify)) {
    gpointer old_data = task->priv->enter_user_data;

    task->priv->enter_user_data = NULL;
    task->priv->enter_notify = NULL;
    GST_OBJECT_UNLOCK (task);

    old_notify (old_data);

    GST_OBJECT_LOCK (task);
  }
  task->priv->enter_func = enter_func;
  task->priv->enter_user_data = user_data;
  task->priv->enter_notify = notify;
  GST_OBJECT_UNLOCK (task);
}

/**
 * @name: gst_task_set_leave_callback:
 * @task: 要使用的 #GstTask
 * @leave_func: （输入）：一个 #GstTaskThreadFunc
 * @user_data: 传递给 @leave_func 的用户数据
 * @notify: 在 @user_data 不再被引用时调用
 *
 * 在离开 @task 的任务函数时调用 @leave_func。@user_data 将被传递给 @leave_func，并且在 @user_data 不再被引用时将调用 @notify。
 */
void
gst_task_set_leave_callback (GstTask * task, GstTaskThreadFunc leave_func,
    gpointer user_data, GDestroyNotify notify)
{
  GDestroyNotify old_notify;

  g_return_if_fail (task != NULL);
  g_return_if_fail (GST_IS_TASK (task));

  GST_OBJECT_LOCK (task);
  if ((old_notify = task->priv->leave_notify)) {
    gpointer old_data = task->priv->leave_user_data;

    task->priv->leave_user_data = NULL;
    task->priv->leave_notify = NULL;
    GST_OBJECT_UNLOCK (task);

    old_notify (old_data);

    GST_OBJECT_LOCK (task);
  }
  task->priv->leave_func = leave_func;
  task->priv->leave_user_data = user_data;
  task->priv->leave_notify = notify;
  GST_OBJECT_UNLOCK (task);
}

/**
 * gst_task_get_state:
 * @task: The #GstTask to query
 *
 * Get the current state of the task.
 *
 * Returns: The #GstTaskState of the task
 *
 * MT safe.
 */
GstTaskState
gst_task_get_state (GstTask * task)
{
  GstTaskState result;

  g_return_val_if_fail (GST_IS_TASK (task), GST_TASK_STOPPED);

  result = GET_TASK_STATE (task);

  return result;
}

/* make sure the task is running and start a thread if it's not.
 * This function must be called with the task LOCK. */
static gboolean
start_task (GstTask * task)
{
  gboolean res = TRUE;
  GError *error = NULL;
  GstTaskPrivate *priv;

  priv = task->priv;

  /* new task, We ref before so that it remains alive while
   * the thread is running. */
  gst_object_ref (task);
  /* mark task as running so that a join will wait until we schedule
   * and exit the task function. */
  task->running = TRUE;

  /* push on the thread pool, we remember the original pool because the user
   * could change it later on and then we join to the wrong pool. */
  priv->pool_id = gst_object_ref (priv->pool);
  priv->id =
      gst_task_pool_push (priv->pool_id, (GstTaskPoolFunction) gst_task_func,
      task, &error);

  if (error != NULL) {
    g_warning ("failed to create thread: %s", error->message);
    g_error_free (error);
    res = FALSE;
  }
  return res;
}

static inline gboolean
gst_task_set_state_unlocked (GstTask * task, GstTaskState state)
{
  GstTaskState old;
  gboolean res = TRUE;

  GST_DEBUG_OBJECT (task, "Changing task %p to state %d", task, state);

  if (state != GST_TASK_STOPPED)
    if (G_UNLIKELY (GST_TASK_GET_LOCK (task) == NULL))
      goto no_lock;

  /* if the state changed, do our thing */
  old = GET_TASK_STATE (task);
  if (old != state) {
    SET_TASK_STATE (task, state);
    switch (old) {
      case GST_TASK_STOPPED:
        /* If the task already has a thread scheduled we don't have to do
         * anything. */
        if (G_UNLIKELY (!task->running))
          res = start_task (task);
        break;
      case GST_TASK_PAUSED:
        /* when we are paused, signal to go to the new state */
        GST_TASK_SIGNAL (task);
        break;
      case GST_TASK_STARTED:
        /* if we were started, we'll go to the new state after the next
         * iteration. */
        break;
    }
  }

  return res;

  /* ERRORS */
no_lock:
  {
    GST_WARNING_OBJECT (task, "state %d set on task without a lock", state);
    g_warning ("task without a lock can't be set to state %d", state);
    return FALSE;
  }
}


/**
 * gst_task_set_state:
 * @task: a #GstTask
 * @state: the new task state
 *
 * Sets the state of @task to @state.
 *
 * The @task must have a lock associated with it using
 * gst_task_set_lock() when going to GST_TASK_STARTED or GST_TASK_PAUSED or
 * this function will return %FALSE.
 *
 * MT safe.
 *
 * Returns: %TRUE if the state could be changed.
 */
gboolean
gst_task_set_state (GstTask * task, GstTaskState state)
{
  gboolean res = TRUE;

  g_return_val_if_fail (GST_IS_TASK (task), FALSE);

  GST_OBJECT_LOCK (task);
  res = gst_task_set_state_unlocked (task, state);
  GST_OBJECT_UNLOCK (task);

  return res;
}

/**
 * @name: gst_task_start
 * @param task: 要启动的 #GstTask
 * @brief: 启动 @task。@task 必须使用 gst_task_set_lock() 关联一个锁，否则此函数将返回 %FALSE。
 * @return: 如果任务能够启动，则为 %TRUE。
 *
 * MT safe
 */
gboolean
gst_task_start (GstTask * task)
{
  return gst_task_set_state (task, GST_TASK_STARTED);
}


/**
 * @name: gst_task_stop
 * @param task: 要停止的 #GstTask
 * @brief: 停止 @task。此方法仅安排任务停止，并不会等待任务完全停止。使用 gst_task_join() 来停止并等待完成。
 * @return: 如果任务能够停止，则为 %TRUE。
 *
 * MT safe
 */
gboolean
gst_task_stop (GstTask * task)
{
  return gst_task_set_state (task, GST_TASK_STOPPED);
}


/**
 * @name: gst_task_pause
 * @param task: 要暂停的 #GstTask
 * @brief: 暂停 @task。这个方法也可以在停止状态下调用，此时将启动一个线程并保持暂停状态。此函数不会等待任务完成暂停状态。
 * @return: 如果任务能够停止，则为 %TRUE。
 *
 * MT safe
 */
gboolean
gst_task_pause (GstTask * task)
{
  return gst_task_set_state (task, GST_TASK_PAUSED);
}


/**
 * @name: gst_task_resume
 * @param task: 要恢复的 #GstTask
 * @brief: 恢复 @task。如果任务是停止状态stopped，将返回FALSE。只有暂停Pause状态，才能恢复。
 * @return: 如果任务能够恢复，则为 %TRUE。
 *
 * MT safe
 */
gboolean
gst_task_resume (GstTask * task)
{
  gboolean res = FALSE;
  g_return_val_if_fail (GST_IS_TASK (task), FALSE);

  GST_OBJECT_LOCK (task);
  if (GET_TASK_STATE (task) != GST_TASK_STOPPED)
    res = gst_task_set_state_unlocked (task, GST_TASK_STARTED);
  GST_OBJECT_UNLOCK (task);

  return res;
}


/**
 * @name: gst_task_join
 * @param task: 要加入的 #GstTask
 * @brief: 加入 @task。在此调用之后，可以安全地取消引用任务，并清理使用 gst_task_set_lock() 设置的锁。
 *         如果调用此函数，任务将会自动被停止
 * @note: 此函数不能从任务函数内部调用，因为这会导致死锁。函数将检测到这一点并打印一个 g_warning。
 * @return: 如果任务能够被加入，则为 %TRUE。
 *
 * MT safe
 */
gboolean
gst_task_join (GstTask * task)
{
  GThread *tself;
  GstTaskPrivate *priv;
  gpointer id;
  GstTaskPool *pool = NULL;

  g_return_val_if_fail (GST_IS_TASK (task), FALSE);

  priv = task->priv;

  tself = g_thread_self ();

  GST_DEBUG_OBJECT (task, "Joining task %p, thread %p", task, tself);

  /* we don't use a real thread join here because we are using
   * thread pools */
  GST_OBJECT_LOCK (task);
  if (G_UNLIKELY (tself == task->thread))
    goto joining_self;
  SET_TASK_STATE (task, GST_TASK_STOPPED);
  /* signal the state change for when it was blocked in PAUSED. */
  GST_TASK_SIGNAL (task);
  /* we set the running flag when pushing the task on the thread pool.
   * This means that the task function might not be called when we try
   * to join it here. */
  while (G_LIKELY (task->running))
    GST_TASK_WAIT (task);
  /* clean the thread */
  task->thread = NULL;
  /* get the id and pool to join */
  pool = priv->pool_id;
  id = priv->id;
  priv->pool_id = NULL;
  priv->id = NULL;
  GST_OBJECT_UNLOCK (task);

  if (pool) {
    if (id)
      gst_task_pool_join (pool, id);
    gst_object_unref (pool);
  }

  GST_DEBUG_OBJECT (task, "Joined task %p", task);

  return TRUE;

  /* ERRORS */
joining_self:
  {
    GST_WARNING_OBJECT (task, "trying to join task from its thread");
    GST_OBJECT_UNLOCK (task);
    g_warning ("\nTrying to join task %p from its thread would deadlock.\n"
        "You cannot change the state of an element from its streaming\n"
        "thread. Use g_idle_add() or post a GstMessage on the bus to\n"
        "schedule the state change from the main thread.\n", task);
    return FALSE;
  }
}