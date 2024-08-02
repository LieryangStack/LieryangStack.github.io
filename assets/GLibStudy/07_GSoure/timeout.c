#include<glib.h>

gboolean 
timeout_cb (gpointer user_data) {
  g_print ("%s\n", __func__);
  return FALSE; /* false删除该超时事件源， source的引用计数 -1 */
}

int
main(int argc, char *argv[]){
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  GMainContext *context = g_main_loop_get_context(loop);

  
  GSource *source = g_timeout_source_new (100);
  g_source_set_callback (source, timeout_cb, NULL, NULL);
  g_source_attach (source, context); /* 此时引用计数 +1， 目前引用计数 == 2 */
  g_source_unref (source);

  g_timeout_add_seconds (1, (GSourceFunc)g_main_loop_quit, loop);

  g_main_loop_run(loop);

  /* 此时 source 对象内存已经被释放 引用计数为零 */

  g_main_context_unref(context);
  g_main_loop_unref(loop);

    

  return 0;
}

