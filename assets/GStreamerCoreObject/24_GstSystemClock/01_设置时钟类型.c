#include <gst/gst.h>

int 
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  /* 设置 "clock-type" 属性，可以从不同方式获取运行时间 
   * 默认是GST_CLOCK_TYPE_MONOTONIC： 系统开机运行了多长时间（避免了修改系统时间而造成时钟混乱）
   */
  GstClock *clock = g_object_new (GST_TYPE_SYSTEM_CLOCK,
                                  "name", "Clock",
                                  "clock-type", GST_CLOCK_TYPE_REALTIME, NULL);
  gst_object_ref_sink (clock);

  GstClockTime time = gst_clock_get_time (clock);
  g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(time));

  gst_object_unref (clock);

  return 0;
}

