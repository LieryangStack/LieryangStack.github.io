#include <gst/gst.h>

#define PRINTF_ERROR(error) \
if (error) { \
  g_warning ("%s\n", error->message); \
  g_clear_error (&error); \
}

static gboolean flag = FALSE;
static GRecMutex task_mutex;

static void 
func (void *user_data) {
  g_print ("%s\n",__func__);
  g_print ("%s\n", (char *)user_data);
  flag = TRUE;
}

int 
main (int argc, char *argv[]) {

  GError *error = NULL;
  char *user_data = "hello";

  gst_init (&argc, &argv);

  g_rec_mutex_init (&task_mutex);

  GstTask* task = gst_task_new (func, user_data, NULL);

  /**
   * 一定要在开始执行任务前，设定任务锁，该锁会在执行任务线程的时候，给任务上锁
   */
  gst_task_set_lock (task, &task_mutex);

  gboolean res = gst_task_set_state (task, GST_TASK_STARTED);

  g_usleep (1000); /* 休眠，让任务线程执行一段时间 */

  gst_task_join (task);

  gst_object_unref (task);

  return 0;
}

