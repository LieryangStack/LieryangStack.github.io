#include "signal-demo.h"

G_DEFINE_TYPE (SignalDemo, signal_demo, G_TYPE_OBJECT);

static void
signal_demo_default_handler(gpointer instance, const gchar *buffer, gpointer userdata)
{
  g_printf("Default handler said: %s\n", buffer);
}

void signal_demo_init(SignalDemo *self)
{

}

void
signal_demo_class_init(SignalDemoClass *klass)
{
  klass->default_handler=signal_demo_default_handler;

  g_signal_new ("hello",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SignalDemoClass, default_handler),
                NULL,
                NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE,
                1, 
                G_TYPE_STRING);
}