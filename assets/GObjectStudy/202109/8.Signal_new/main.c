#include "signal-demo.h"

static void
my_signal_handler(gpointer *instance, gchar *buffer, gpointer userdata)
{
  g_printf ("my_signal_handler said: %s\n", buffer);
  g_printf ("my_signal_handler said: %s\n", (gchar *)userdata);
}

int
main(void)
{
  g_type_init();
  gchar *userdata = "This is userdata";
  SignalDemo *sd_obj = g_object_new(SIGNAL_TYPE_DEMO, NULL);

  /* 信号连接 */
  g_signal_connect(sd_obj, "hello",
                   G_CALLBACK (my_signal_handler),
                   userdata);

  /* 发射信号 */
  g_signal_emit_by_name (sd_obj, 
                          "hello", 
                          "This is the second param", 
                          G_TYPE_NONE);

  return 0;
}