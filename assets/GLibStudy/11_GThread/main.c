#include <glib.h>

typedef struct _GRealThread GRealThread;
struct  _GRealThread
{
  GThread thread;

  gint ref_count;
  gboolean ours;
  gchar *name;
  gpointer retval;
};

GThread *thread = NULL;

static gpointer
execute_thread (gpointer data) {

  GRealThread *real = (GRealThread *) thread;
  g_print ("%s:real->ref_count = %d\n", __func__, real->ref_count);

  g_thread_exit ("manual exit"); /* 这里直接就退出了 */

  g_print ("%s:exit\n", __func__);

  return "success return";
}

int 
main(int argc, char const *argv[])
{
  GError *error = NULL;
  
  /* 线程刚创建完毕，此时 real->ref_count == 2 */
  thread = g_thread_try_new ("test thread", execute_thread, NULL, &error);
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_error_free (error);
    return -1;
  }

  /* 延时等待之后，子线程函数已经执行完毕，此时 real->ref_count == 1 */
  g_usleep (G_USEC_PER_SEC / 100);

  GRealThread *real = (GRealThread *) thread;
  g_print ("%s:real->ref_count = %d\n", __func__, real->ref_count);

  /* 里面会调用 g_thread_unref，无需手动再次调用 unref */
  gpointer thread_ret = g_thread_join (thread);
  if (thread_ret)
    g_print ("thread_ret = %s\n", (gchar *)thread_ret);

  g_print ("%s:real->ref_count = %d\n", __func__, real->ref_count);

  return 0;
}
