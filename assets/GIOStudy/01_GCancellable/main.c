/***
 * @brief: 使用 GCancellable 同时取消多个线程执行
*/

#include <glib.h>
#include <gio/gio.h>
#include <gobject/gobject.h>

GCancellable *cancellable = NULL;

static void
thread_cancelled2quit_cb (GCancellable *cancellable,
                          gpointer      user_data) {
  g_print ("%s:%p\n",__func__, g_thread_self());
  g_main_loop_quit ((GMainLoop *)user_data);
}

static gpointer 
thread_func (gpointer data) {
  g_print ("%s:%p\n",__func__, g_thread_self());

  GMainContext *context = g_main_context_new ();
  GMainLoop *loop = g_main_loop_new (context, TRUE);
  g_main_context_unref (context);

  gulong id = g_cancellable_connect (cancellable, (GCallback)thread_cancelled2quit_cb, loop, NULL);

  g_main_loop_run (loop); 

  g_cancellable_disconnect (cancellable, id);

  g_main_loop_unref (loop);

  return 0;
}


int
main (int argc, char *argv[]) {

  g_print ("%s:%p\n",__func__, g_thread_self());

  GMainLoop *loop = g_main_loop_new (NULL, TRUE);

  cancellable = g_cancellable_new ();
  gulong id = g_cancellable_connect (cancellable, (GCallback)thread_cancelled2quit_cb, loop, NULL);

  GThread *thread1 = g_thread_try_new ("thread1", thread_func, NULL, NULL);
  GThread *thread2 = g_thread_try_new ("thread2", thread_func, NULL, NULL);

  /* 信号在那个线程发射，信号回调函数就在那个线程中执行 */
  g_timeout_add (1, (GSourceFunc)g_cancellable_cancel, cancellable);

  g_main_loop_run (loop);

  g_thread_join (thread1);
  g_thread_join (thread2);

  /* 此时引用计数 cancellable->ref_count = 1  
   * 连接信号不会增加引用计数
   */
  g_print ("cancellable->ref_count = %d\n", ((GObject *)cancellable)->ref_count);

  /* 清理资源 */
  g_cancellable_disconnect (cancellable, id);
  g_main_loop_unref (loop);
  g_object_unref (cancellable);

  return 0;
}