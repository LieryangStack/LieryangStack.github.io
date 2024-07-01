#include <gst/gst.h>

int 
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  GstClock *static_clock = gst_system_clock_obtain ();
  GstClock *clock = g_object_new (GST_TYPE_SYSTEM_CLOCK, "name", "Clock", "clock-type", GST_CLOCK_TYPE_REALTIME, NULL);
  gst_object_ref_sink (clock);

  GstClockTime time = gst_clock_get_time (clock);

  GstClockTime internal, external;
  internal = gst_clock_get_internal_time (clock);
  external = gst_clock_get_time (clock);
  g_print("internal time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(internal));
  g_print("external time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(external));

  internal = gst_clock_get_internal_time (clock);
  external = gst_clock_get_time (clock);
  g_print("internal time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(internal));
  g_print("external time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(external));、

  GST_OBJECT_FLAG_SET (clock, GST_CLOCK_FLAG_NEEDS_STARTUP_SYNC);

  gboolean ret = gst_clock_is_synced (clock);
  g_print  ("ret = %d\n", ret);

  gst_clock_set_synced (clock, FALSE);

  gst_clock_wait_for_sync (clock, 3 * GST_SECOND);

  // g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));

  // while (1) {
  //   time = gst_clock_get_time (clock);
  //   g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));
  // }

  gst_object_unref (clock);

  return 0;
}

