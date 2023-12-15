#include "signal-demo.h"

G_DEFINE_TYPE (SignalDemo, signal_demo, G_TYPE_OBJECT);

#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer

void
g_cclosure_user_marshal_INT__VOID_STRING (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint G_GNUC_UNUSED,
                                          gpointer      marshal_data)
{
  typedef gint (*GMarshalFunc_INT__VOID_STRING) (gpointer data1,
                                                 gpointer arg1,
                                                 gpointer data2);
  GCClosure *cc = (GCClosure *) closure;
  gpointer data1, data2;
  GMarshalFunc_INT__VOID_STRING callback;
  gint v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_INT__VOID_STRING) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       data2);

  g_value_set_int (return_value, v_return);
}

gboolean
signal_demo_accumulator (GSignalInvocationHint *ihint,
                         GValue *return_accu,
                         const GValue *handler_return,
                         gpointer data)
{
        g_print ("%d\n", g_value_get_int (handler_return));
        g_print ("%s\n", (gchar *)data);
         
        return TRUE;
}
 
static gint
signal_demo_default_handler (gpointer instance, const gchar *buffer, gpointer userdata)
{
        g_printf ("Default handler said: %s\n", buffer);
        return 2;
}
 
static void
signal_demo_init (SignalDemo *self)
{
}
 
void
signal_demo_class_init (SignalDemoClass *klass)
{
        klass->default_handler = signal_demo_default_handler;
         
        g_signal_new ("hello",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (SignalDemoClass, default_handler),
                      signal_demo_accumulator,
                      "haha haha",
                      g_cclosure_user_marshal_INT__VOID_STRING,
                      G_TYPE_INT,
                      1,
                      G_TYPE_STRING);
}