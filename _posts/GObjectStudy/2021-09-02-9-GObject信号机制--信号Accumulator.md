---
layout: post
title: 九、GObject信号机制--信号Accumulator
categories: GObject学习笔记
tags: [GObject学习笔记]
---

## 1 类设计
```c
/* file name : signal-demo.h */
#ifndef SIGNAL_DEMO_H
#define SIGNAL_DEMO_H

#include <glib-object.h>

#define SIGNAL_TYPE_DEMO (signal_demo_get_type ())
#define SIGNAL_DEMO(object) \
        G_TYPE_CHECK_INSTANCE_CAST ((object), SIGNAL_TYPE_DEMO, SignalDemo)
#define SIGNAL_IS_DEMO(object) \
        G_TYPE_CHECK_INSTANCE_TYPE ((object), SIGNAL_TYPE_DEMO))
#define SIGNAL_DEMO_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST ((klass), SIGNAL_TYPE_DEMO, SignalDemoClass))
#define SIGNAL_IS_DEMO_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass), SIGNAL_TYPE_DEMO))
#define SIGNAL_DEMO_GET_CLASS(object) (\
                G_TYPE_INSTANCE_GET_CLASS ((object), SIGNAL_TYPE_DEMO, SignalDemoClass))

typedef struct _SignalDemo SignalDemo;
struct _SignalDemo {
  GObject parent;
};

typedef struct _SignalDemoClass SignalDemoClass;
struct _SignalDemoClass {
  GObjectClass parent_class;
  void (*default_handler) (gpointer instance, const gchar *buffer, gpointer userdata);
};

GType signal_demo_get_type (void);

#endif
```

```c
/* file name : signal-demo.c */
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
```

```c
/* file name : main.c */
#include "signal-demo.h"
  
static gint
my_signal_handler (gpointer *instance, gchar *buffer, gpointer userdata)
{
        g_print ("my_signal_handler said: %s\n", buffer);
        g_print ("my_signal_handler said: %s\n", (gchar *)userdata);
 
    return 1;
}
  
int
main (void)
{
        g_type_init ();
         
        gchar *userdata = "This is userdata";
        SignalDemo *sd_obj = g_object_new (SIGNAL_TYPE_DEMO, NULL);
         
        gint val = 0;
         
        /* 信号连接 */
        g_signal_connect (sd_obj, "hello",
                          G_CALLBACK (my_signal_handler),
                          userdata);
  
        /* 发射信号 */
        g_signal_emit_by_name (sd_obj,
                               "hello",
                               "This is the second param", &val);
 
        return 0;
}
```




## 2 分析
```c
guint      g_signal_new (const gchar        *signal_name,
					     GType               itype,
					     GSignalFlags        signal_flags,
					     guint               class_offset,
					     GSignalAccumulator	 accumulator,
					     gpointer		 accu_data,
					     GSignalCMarshaller  c_marshaller,
					     GType               return_type,
					     guint               n_params,
					     ...);
```
Param 5：是一个类型为GSignalAccumulator函数指针，即

```c
typedef gboolean (*GSignalAccumulator)	(GSignalInvocationHint *ihint,
					 GValue		       *return_accu,
					 const GValue	       *handler_return,
					 gpointer               data);
```
&emsp;&emsp;g_signal_new的第六个参数accu_data会被传入到GSignalAccumulator所指向函数的最后一个参数，即data。
&emsp;&emsp;accumulator所指向的函数会在信号所连接的**每个闭包**（一共两个闭包，一个是信号注册阶段连接的闭包；另一个是信号连接阶段中使用者所提供的闭包）**运行完成后被调用**，它的主要功能就是收集信号所连接的各个闭包的返回值（简称”信号返回值“）。
&emsp;&emsp;signal-demo.c文件中添写g_cclosure_user_marshal...函数，主要是因为accumulator要求闭包函数要有返回值。该函数可以通过闭包章节讲的命令生成。

```c
void
g_cclosure_user_marshal_INT__VOID_STRING (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint G_GNUC_UNUSED,
                                          gpointer      marshal_data)
```
