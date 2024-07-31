#include <gtk/gtk.h>

int
main (int argc, char **argv) {
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;

  gint a = 50;

  /* 创建一个常量表达式 */
  expression = gtk_constant_expression_new (G_TYPE_INT,100);

  /* 从表达式中获取存储的GValue 
   * this 的功能属性表达式里面讲解
   */
  if (gtk_expression_evaluate (expression, NULL, &value))
    g_print ("The value is %d.\n", g_value_get_int (&value));
  else
    g_print ("The constant expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);

  return 0;
}