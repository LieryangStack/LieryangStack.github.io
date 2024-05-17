#include <glib.h>
#include <gio/gio.h>
#include <gobject/gobject.h>

int
main (int argc, char *argv[]) {

  
  GMainLoop *loop = g_main_loop_new (NULL, TRUE);


   
  GCancellable *cancellable = g_cancellable_new ();


  g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

  g_main_loop_run (loop);

  g_main_loop_unref (loop);
  g_object_unref (cancellable);

  GTask

  return 0;
}