#include <glib.h>
#include <gio/gio.h>

static void 
ready_callback (GObject *source_object,
                GAsyncResult *res,
                gpointer data) {

  g_print ("%s\n", __func__);
  g_main_loop_quit ((GMainLoop *)data);
}

static void 
task_thread_func (GTask           *task,
                  gpointer         source_object,
                  gpointer         task_data,
                  GCancellable    *cancellable) {
  g_print ("%s\n", __func__);
}

int
main (int argc, char *argv[]) {

  
  GCancellable *cancellable = g_cancellable_new ();
  GObject *test_object = g_object_new (G_TYPE_OBJECT, NULL);
  GMainLoop *loop = g_main_loop_new (NULL, TRUE);

  // g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

  GTask *task = g_task_new (test_object, cancellable, ready_callback, loop);
  g_task_set_task_data(task, g_strdup("task_data"), g_free);
	g_task_run_in_thread(task, task_thread_func);

  g_main_loop_run (loop);

  g_print ("task->ref_count = %d\n", ((GObject *)task)->ref_count);

  g_object_unref (cancellable);
  g_object_unref (test_object);
  g_object_unref (task);
  g_main_loop_unref (loop);
  

  

  return 0;
}