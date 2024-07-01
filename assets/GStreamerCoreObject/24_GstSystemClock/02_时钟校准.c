#include <gst/gst.h>

int 
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  GstClockTime cinternal, cexternal, crate_num, crate_denom;

  GstClock *clock = g_object_new (GST_TYPE_SYSTEM_CLOCK, "name", "Clock", NULL);
  gst_object_ref_sink (clock);

  gst_clock_get_calibration (clock, &cinternal, &cexternal,
      &crate_num, &crate_denom);

  g_print ("cinternal = %ld\n", cinternal); /* 默认 cinternal = 0  */
  g_print ("cexternal = %ld\n", cexternal); /* 默认 cinternal = 0  */
  g_print ("crate_num = %ld\n", crate_num); /* 默认 crate_num = 1  */
  g_print ("crate_denom = %ld\n", crate_denom); /* 默认 crate_denom = 1  */

  GstClockTime internal, external;
  internal = gst_clock_get_internal_time (clock);
  external = gst_clock_get_time (clock);

  /**
   * 我感觉意义不大，external - internal 的间隔也就是这两个函数执行时间间隔。
   */
  gst_clock_set_calibration (clock, internal, external, 1, 1);

  GstClockTime time = gst_clock_get_time (clock);
  g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));


  gst_object_unref (clock);

  return 0;
}

