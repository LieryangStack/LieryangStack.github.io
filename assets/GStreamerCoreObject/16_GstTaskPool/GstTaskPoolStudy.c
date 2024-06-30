#include <gst/gst.h>

#define PRINTF_ERROR(error) \
if (error) { \
  g_warning ("%s\n", error->message); \
  g_clear_error (&error); \
}

static gboolean flag = FALSE;

static void 
pool_func (void *user_data) {
  g_print ("%s\n", (char *)user_data);
  flag = TRUE;
}

int 
main (int argc, char *argv[]) {

  GError *error = NULL;

  gst_init (&argc, &argv);

  /* 只是新建了一个 GstTaskPool 对象， 并没有给 task_pool->pool创建 */
  GstTaskPool *task_pool = gst_task_pool_new ();

  /* 如果没有预先调用 gst_task_pool_prepare 函数，系统是不会给 task_pool->pool 创建线程池 */
  gst_task_pool_push (task_pool, pool_func, "hello", &error);

  PRINTF_ERROR (error);

  /**
   * 必须要 prepare 才能创建线程池
   */
  gst_task_pool_prepare (task_pool, &error);

  PRINTF_ERROR (error);

  gst_task_pool_push (task_pool, pool_func, "hello", &error);

  PRINTF_ERROR (error);

  /* 需要循环等待一下，防止线程池任务没有执行完毕 */
  while (flag != TRUE);

  /**
   * 释放线程池
   */
  gst_task_pool_cleanup (task_pool);

  gst_object_unref (task_pool);

  return 0;
}

