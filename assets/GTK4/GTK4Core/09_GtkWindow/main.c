#include <adwaita.h>

GtkWidget *win;

static void 
app_activate (GApplication *app, gpointer *user_data) {

  win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 500, 400);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("标签");

  GtkWidget *popover = gtk_popover_new ();

  gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  GtkNative *native = NULL;

  /* GtkWindow 和 GtkPopover 实现了GtkNative接口*/
  g_print ("GTK_NATIVE (win) = %p\n", GTK_NATIVE (win));
  g_print ("GTK_NATIVE (popover) = %p\n", GTK_NATIVE (popover));

  /* 这些子构件没有实现 GdkNative 接口 */
  g_print ("GTK_NATIVE (box) = %p\n", GTK_NATIVE (box));
  g_print ("GTK_NATIVE (button) = %p\n", GTK_NATIVE (button));
  g_print ("GTK_NATIVE (label) = %p\n", GTK_NATIVE (label));

  /* 同一个GtkWindow内部都是共同的GdkNavie，也就是共同的GdkSurface */
  native = gtk_widget_get_native (win);
  native = gtk_widget_get_native (box);
  native = gtk_widget_get_native (button);
  native = gtk_widget_get_native (label);

  /* 只有内部调用了 gtk_widget_realize () 函数，才会创建Surface */
  gtk_window_present (GTK_WINDOW (win));

  /* 通过GdkNative可以获取 GdkSurface */
  GskRenderer *render =  gtk_native_get_renderer (gtk_widget_get_native(win));
  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  /* 两种获取 GdkDisplay 的方式 */
  GdkDisplay *display = gdk_surface_get_display (surface);
  GdkDisplay *default_display = gdk_display_get_default ();

  /* 获取显示后端 */
  const gchar *name = gdk_display_get_name (default_display);
  g_print ("name = %s\n", name);

  /* 获取渲染方式 */
  g_print ("G_OBJECT_TYPE_NAME (renderer) = %s\n", G_OBJECT_TYPE_NAME (render));
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}