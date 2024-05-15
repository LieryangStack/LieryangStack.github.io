#include <glib.h>

typedef struct {
  GMutex mutex;
  GCond cond;
  gboolean signalled;
} MutexCond;

static void
pool_func (gpointer data, gpointer user_data) {
  g_print ("%s\n", __func__);

  GThreadPool *pool = (GThreadPool *)data;

  gint unprocessed = g_thread_pool_unprocessed (pool);
  guint num_threads_runing = g_thread_pool_get_num_threads(pool);

  // while ( 1)
  // {
  //   /* code */
  // }
  
  g_print ("unprocessed = %d\n", unprocessed);
  g_print ("num_threads_runing = %d\n", num_threads_runing);
}

/**
 * @brief: 看独有
*/
static void
test_exclusive_true () {
  GThreadPool *pool = g_thread_pool_new (pool_func, NULL, 2, FALSE, NULL);

  gint unprocessed = g_thread_pool_unprocessed (pool);
  guint num_threads_runing = g_thread_pool_get_num_threads(pool);
  gint max_threads = g_thread_pool_get_max_threads (pool);

  g_print ("unprocessed = %d\n", unprocessed);
  g_print ("num_threads_runing = %d\n", num_threads_runing);
  g_print ("max_threads = %d\n", max_threads);

  g_thread_pool_push (pool, pool, NULL);

  g_usleep (G_USEC_PER_SEC * 1);

  // g_thread_pool_push (pool, pool, NULL);
  // g_thread_pool_push (pool, pool, NULL);
  // g_thread_pool_push (pool, pool, NULL);

  pool = g_thread_pool_new (pool_func, NULL, 2, FALSE, NULL);
  g_thread_pool_push (pool, pool, NULL);


  g_usleep (G_USEC_PER_SEC * 1);

  g_thread_pool_free (pool, TRUE, TRUE);
}

static void
test_simple (gconstpointer shared)
{
  GThreadPool *pool;
  GError *err = NULL;
  MutexCond m;
  gboolean success;

  g_mutex_init (&m.mutex);
  g_cond_init (&m.cond);

  if (GPOINTER_TO_INT (shared)) {
    g_test_summary ("Tests that a shared, non-exclusive thread pool "
                    "generally works.");
    pool = g_thread_pool_new (pool_func, &m, -1, FALSE, &err);
  } else {
    g_test_summary ("Tests that an exclusive thread pool generally works.");
    pool = g_thread_pool_new (pool_func, &m, 2, TRUE, &err);
  }

  g_assert_no_error (err);
  g_assert_nonnull (pool);

  g_mutex_lock (&m.mutex);
  m.signalled = FALSE;
  g_print ("g_thread_pool_push\n");
  success = g_thread_pool_push (pool, GUINT_TO_POINTER (123), &err);
  g_assert_no_error (err);
  g_assert_true (success);

  while (!m.signalled)
    g_cond_wait (&m.cond, &m.mutex);
  g_mutex_unlock (&m.mutex);

  g_thread_pool_free (pool, TRUE, TRUE);
}


int
main (int argc, char *argv[]) {
  
  test_exclusive_true ();



  return 0;
}