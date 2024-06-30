#include <gst/gst.h>

int 
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  GstClock *static_clock = gst_system_clock_obtain ();

  GstClockTime time = gst_clock_get_time (static_clock);

  g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));

  while (1) {
    time = gst_clock_get_time (static_clock);
    g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));
  }


  return 0;
}

