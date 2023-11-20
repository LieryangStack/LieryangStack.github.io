/**
  Compile:
      gcc Gvalue.c `pkg-config --cflags --libs glib-2.0 gobject-2.0` 
*/
#include<glib.h>
#include<glib/gprintf.h>
#include<glib-object.h>
#include<stdlib.h>

static void
string2int (const GValue *src_value,
            GValue       *dest_value)
{
    const gchar *value = g_value_get_string (src_value);
    int i = atoi(value);
    g_value_set_int (dest_value, i);
}

int 
main (int argc, char *argv[]) {
  /*Gvalues 一定要被初始化（因为内部用的是联合体，所谓初始化就是内部空间全赋值为0）*/
  GValue a = G_VALUE_INIT;
  GValue b = G_VALUE_INIT;
  
  /* g_assert判断是否是G_VALUE_HOLDS_STRING字符串类型
   * 因为现在和没有初始化类型，G_VALUE_HOLDS_STRING(&a) 等于 0
   */
  g_assert(!G_VALUE_HOLDS_STRING(&a));

  /*把变量a初始化为字符串类型*/
  g_value_init(&a, G_TYPE_STRING);
  g_assert(G_VALUE_HOLDS_STRING(&a));
  g_value_set_string(&a, "Hello World");
  g_printf("%s\n", g_value_get_string(&a));
  
  /* 必须重置变量a恢复到原始状态 */
  g_value_unset(&a);

  /* 必须初始化成uint64，否则报错 */
  g_value_init(&a, G_TYPE_INT);
  g_value_set_uint64(&a, 12);
  g_printf("%ld\n", g_value_get_uint64(&a));

  g_value_init(&b, G_TYPE_STRING);
  g_assert(g_value_type_transformable(G_TYPE_INT, G_TYPE_STRING));

  g_value_transform(&a, &b);
  g_printf("%s\n", g_value_get_string(&b));

  /* An STRINT is not transformable to INT */
  if (g_value_type_transformable (G_TYPE_STRING, G_TYPE_INT)) {
      g_printf ("Can transform string to int\n");
  } else
      g_printf ("Can Not transform string to int\n");
  

/* Attempt to transform it again using a custom transform function */
  g_value_set_string(&b, "43");
  g_value_register_transform_func (G_TYPE_STRING, G_TYPE_INT, string2int);
  g_value_transform (&b, &a);
  g_printf ("%d\n", g_value_get_int(&a));

  return 0;
}
