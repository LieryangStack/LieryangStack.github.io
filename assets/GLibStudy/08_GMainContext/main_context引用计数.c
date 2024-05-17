#include <glib.h>

typedef struct _Test_GMainContext Test_GMainContext;
struct _Test_GMainContext {
  /* The following lock is used for both the list of sources
   * and the list of poll records
   */
  GMutex mutex;
  GCond cond;
  GThread *owner;
  guint owner_count;
  GMainContextFlags flags;
  GSList *waiters;

  gint ref_count;  /* (atomic) */
};

typedef struct _Test_GMainLoop  Test_GMainLoop;
struct _Test_GMainLoop 
{
  GMainContext *context;
  gboolean is_running; /* (atomic) */
  gint ref_count;  /* (atomic) */
};


int
main (int argc, char *argv[]) {

  GMainContext *context = g_main_context_new (); /* 引用计数 context->ref_count = 1 */
  GMainLoop *loop = g_main_loop_new(context, TRUE); /* 引用计数 context->ref_count = 2, loop->ref_count = 1 */
  g_main_context_unref (context);

  g_print ("context->ref_count = %d\n", ((Test_GMainContext *)context)->ref_count);

  // g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop); /* 直接添加是不行的，因为函数里面获取的是默认default_main_context */

  GSource *source = g_timeout_source_new_seconds (1);
  g_source_set_callback (source, (GSourceFunc)g_main_loop_quit, loop, NULL);
  g_source_attach (source, context);
  g_source_unref (source); /* 解引用完成之后，此时 source->ref = 1 */

  /* g_main_loop_run循环函数进入时候，引用计数加一， loop->ref_count = 2 
   * g_main_loop_run循环函数最后退出之前，引用计数减一， loop->ref_count = 1
   */
  g_main_loop_run (loop); 

  g_print ("loop->ref_count = %d\n", ((Test_GMainLoop *)loop)->ref_count); /* loop->ref_count = 1 */

  g_main_loop_unref (loop); /* 这里解引用loop的时候，会把context解引用一次，context里面会把source解引用一次 */

  g_print ("main loop quit...\n");
  g_print ("context->ref_count = %d\n", ((Test_GMainContext *)context)->ref_count);
  g_print ("source->ref_count = %d\n", source->ref_count);
  

  return 0;
}