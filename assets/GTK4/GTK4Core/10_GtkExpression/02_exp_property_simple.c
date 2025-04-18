#include <gtk/gtk.h>

int
main (int argc, char **argv) {
  GtkWidget *label;
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;

  gtk_init ();

  label = gtk_label_new ("Hello");
  GtkExpression *another_expression = gtk_constant_expression_new (GTK_TYPE_LABEL, label);


  label = gtk_label_new ("Hello world.");
  
  /**
   * 如果设定另外表达式，evaluate得到的就是另外表达式对象里面存储的值
   * 如果没有设定另外表达式，evaluate得到的就是 @this_ 对象里面的值
   */
  expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");

  // expression = gtk_property_expression_new (GTK_TYPE_LABEL, NULL, "label");
  /* Evaluate the expression */
  if (gtk_expression_evaluate (expression, label, &value))
    g_print ("The value is %s.\n", g_value_get_string (&value));
  else
    g_print ("The property expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);

  return 0;
}