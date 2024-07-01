#include <gst/gst.h>

static gpointer
thread_func (gpointer data) {

  GstClock *clock = GST_CLOCK (data);

  g_usleep (G_USEC_PER_SEC * 1);

  gst_clock_set_synced (clock, TRUE);

  g_print ("sync\n");
  
}

int 
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  GstClock *clock = g_object_new (GST_TYPE_SYSTEM_CLOCK, "name", "Clock", NULL);
  gst_object_ref_sink (clock);

  /* 一定要先设置该 flag */
  GST_OBJECT_FLAG_SET (clock, GST_CLOCK_FLAG_NEEDS_STARTUP_SYNC);
  gst_clock_set_synced (clock, FALSE);

  GThread *thread = g_thread_try_new ("sync-clock", thread_func, clock, NULL);
  
  gst_clock_wait_for_sync (clock, GST_CLOCK_TIME_NONE);

  g_thread_join (thread);

  gst_object_unref (clock);

  return 0;
}

