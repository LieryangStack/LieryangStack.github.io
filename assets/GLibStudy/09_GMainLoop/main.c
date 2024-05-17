#include <glib.h>

int
main (int argc, char *argv[]) {

  /***
   * @param context(nullable): 设置为NULL，使用全局默认上下文，也可以添加自己创建的context
   * @param is_running: 这个设不设置都可以，因为 g_main_loop_run() 会设置 loop->is_running = TRUE
  */
  GMainLoop *loop = g_main_loop_new (NULL, TRUE);

  g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

  g_main_loop_run (loop);

  g_main_loop_unref (loop);

  

  return 0;
}