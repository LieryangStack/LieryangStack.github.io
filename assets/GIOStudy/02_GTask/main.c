#include <glib.h>
#include <gio/gio.h>

/**
 * @brief: 任务执行完毕后，调用的回调函数
*/
static void 
task_complete_callback (GObject *source_object,
                        GAsyncResult *res,
                        gpointer data) {
  g_print ("%s:%p\n", __func__, g_thread_self());

  GError *error = NULL;

  /* 获取任务线程中返回的数据 */
  gint task_ret = g_task_propagate_int ((GTask *)res, &error);

  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_error_free (error);
    goto exit;
  } 

  g_print ("%s:task_ret = %d\n", __func__, task_ret);

exit:

  g_main_loop_quit ((GMainLoop *)data);
}

/***
 * @brief: 任务线程中要执行的函数，该函数是用 GThreadPool执行，该线程池由GTask内部维护。
*/
static void 
task_thread_func (GTask           *task,
                  gpointer         source_object,
                  gpointer         task_data,
                  GCancellable    *cancellable) {
  g_print ("%s:%p\n", __func__, g_thread_self());

  while (1) {
    if (g_cancellable_is_cancelled(cancellable)) /* 如果接收到取消，就退出循环 */
      break;
    
    /* 执行任务 */
  }

  /* 把数据发送到任务对象中，方便其他函数获取线程函数执行结果 */
  g_task_return_int (task, 666);
}

/**
 * @brief: 取消任务执行
*/
static gboolean
timeout_cb (gpointer data) {

  g_return_val_if_fail (G_CANCELLABLE(data), TRUE);

  g_cancellable_cancel (G_CANCELLABLE(data));

  return TRUE;
}

int
main (int argc, char *argv[]) {

  g_print ("%s:%p\n", __func__, g_thread_self());

  
  GCancellable *cancellable = g_cancellable_new ();
  GObject *test_object = g_object_new (G_TYPE_OBJECT, NULL);
  GMainLoop *loop = g_main_loop_new (NULL, TRUE);

  // g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);
  
  GTask *task = g_task_new (test_object, cancellable, task_complete_callback, loop);

  /**
   * 如果 task->check_cancellable == TRUE，任务如果被取消，会报告取消错误，意味着不能使用线程执行函数传播的返回值，因为只要有错误，返回值就是-1。
   * 如果 task->check_cancellable == FALSE，任务如被取消，不会报告取消错误
  */
  g_task_set_check_cancellable (task, FALSE); /* 因为我们线程函数中自行检查是否已经被取消，所以不设置GTask内部去检查 */


  g_task_set_task_data(task, g_strdup("task_data"), g_free);
	g_task_run_in_thread(task, task_thread_func);
  // g_task_run_in_thread(task, task_thread_func);  //任务只能执行一次

  g_timeout_add (100, timeout_cb, cancellable);

  g_main_loop_run (loop);

  g_print ("task->ref_count = %d\n", ((GObject *)task)->ref_count);

  g_print ("task data = %s\n", (char *)g_task_get_task_data (task));

  g_object_unref (cancellable);
  g_object_unref (test_object);
  g_object_unref (task);
  g_main_loop_unref (loop);

  return 0;
}