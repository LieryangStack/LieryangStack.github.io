#include <adwaita.h>

/**
 * @brief: GtkWindow窗口 `close-request` 信号回调处理函数
 * @return: 返回FALSE，系统就会调用 gtk_window_destroy 函数
 *          返回TRUE，系统不会执行任何操作
*/
static gboolean
close_request (GtkWindow* self, gpointer user_data) {

  g_print ("%s\n", __func__);

  static gint ret = FALSE;
  ret = !ret;

  return ret;
}

GtkWidget *win;
static void 
app_activate (GApplication *app, gpointer *user_data) {

  /* GtkWindow实例初始化函数中，进行了浮点引用转正，因为GtkApplicationWindow继承于GtkWindow */
  win = gtk_application_window_new (GTK_APPLICATION (app));
  g_signal_connect (win, "close-request", G_CALLBACK (close_request), NULL);
  // g_signal_connect (win, "close-request", G_CALLBACK (close_request), NULL); /* 暂时不知道多个连接，glib信号系统会如何处理 */

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_window_present (GTK_WINDOW (win));

  g_print ("g_object_is_floating (win) = %d\n", g_object_is_floating (win)); /* 不是附点引用 */
  g_print ("G_OBJECT (win)->ref_count = %d\n", G_OBJECT (win)->ref_count); /* G_OBJECT (win)->ref_count = 1 */
  g_print ("G_OBJECT (app)->ref_count = %d\n", G_OBJECT (app)->ref_count); /* G_OBJECT (app)->ref_count = 4 */
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_print ("G_OBJECT (win)->ref_count = %d\n", G_OBJECT (win)->ref_count);
  g_print ("G_OBJECT (app)->ref_count = %d\n", G_OBJECT (app)->ref_count); /* G_OBJECT (app)->ref_count = 1 */
  g_object_unref (app);

  return 0;
}