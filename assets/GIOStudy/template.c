#include <glib.h>

int
main (int argc, char *argv[]) {

  
  GMainLoop *loop = g_main_loop_new (NULL, TRUE);

  g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

  g_main_loop_run (loop);

  g_main_loop_unref (loop);

  

  return 0;
}