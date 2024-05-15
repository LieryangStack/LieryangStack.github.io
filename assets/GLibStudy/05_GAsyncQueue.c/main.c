#include <glib.h>

typedef struct _TEST_GAsyncQueue TEST_GAsyncQueue;
struct _TEST_GAsyncQueue
{
  GMutex mutex;
  GCond cond;
  GQueue queue;
  GDestroyNotify item_free_func;
  guint waiting_threads;
  gint ref_count;
};

static gpointer
g_thread_thread_proxy (gpointer data) {

  GAsyncQueue *queue = (GAsyncQueue *)data;

  for (gint i = 0; i < 50; i++) {
    g_async_queue_lock (queue);

    /**
     * 如果队列中没有消息，该会执行 g_cond_wait （先解锁，然后进去阻塞等待状态，等有消息会解除阻塞，再上锁）
     * 这个函数会压数据入队，g_async_queue_push_unlocked，检测到有等待线程，发发信号 g_cond_signal
    */
    gpointer p = g_async_queue_pop_unlocked (queue);

    g_async_queue_unlock (queue);

    g_print ("thread = %p, i = %d deal data %d\n", g_thread_self(), i, GPOINTER_TO_INT (p));
    g_usleep (1000);
  }

  return 0;
}

int
main (int argc, char *argv[]) {

  /* 这里创建后 ref_count = 1 */
  GAsyncQueue *queue = g_async_queue_new ();

  GThread *thread1 = g_thread_try_new ("thread1", g_thread_thread_proxy, queue, NULL);
  GThread *thread2 = g_thread_try_new ("thread2", g_thread_thread_proxy, queue, NULL);

  for (gint i = 0; i < 101; i++) {
    g_async_queue_lock (queue);

    g_async_queue_push_unlocked (queue, GINT_TO_POINTER (i));

    g_async_queue_unlock (queue);
  }

  g_thread_join (thread1);
  g_thread_join (thread2);

  /* 除了ref函数，没有其他函数能修改引用计数，所以此时 ref_count == 1 */
  g_print ("ref_count = %d\n", ((TEST_GAsyncQueue *)queue)->ref_count);
  g_async_queue_unref (queue);

  return 0;
}