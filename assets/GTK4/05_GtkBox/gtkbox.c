#include <adwaita.h>

static void 
app_activate (GApplication *app, gpointer *user_data) {

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));


  GtkWidget *box = gtk_box_new (1, 0);

  g_print ("g_object_is_floating (box) = %d\n", g_object_is_floating (box)); /* 是附点引用 */

  gtk_window_set_child (GTK_WINDOW(win), box);

  g_print ("g_object_is_floating (box) = %d\n", g_object_is_floating (box)); /* 转换成正常引用 */

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  

  gtk_window_present (GTK_WINDOW (win));

  /**
   * 此时 box 的引用计数是 1，销毁win的时候，也会对win的child进行解引用
  */
  g_print ("G_OBJECT (box)->ref_count = %d\n", G_OBJECT (box)->ref_count); /* G_OBJECT (win)->ref_count = 1 */
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  /* 此时：G_OBJECT (app)->ref_count = 1 */
  g_object_unref (app);

  return 0;
}