#include <adwaita.h>

static gboolean
timeout_cb (gpointer data) {
  GtkPicture *picture = GTK_PICTURE (data);

  GdkPaintable *paintable = gtk_picture_get_paintable (picture);

  g_print ("%s\n", G_OBJECT_TYPE_NAME (paintable));

  return FALSE;
}

static void 
app_activate (GApplication *app, gpointer *user_data) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 100, 100);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("标签");

  GtkWidget *picture = gtk_picture_new_for_filename ("/home/lieryang/Desktop/LieryangStack.github.io/assets/GTK4/13_GtkImage/image/bird.jpg");

  gtk_picture_set_can_shrink (GTK_PICTURE(picture), FALSE);
  gtk_picture_set_content_fit (GTK_PICTURE(picture), GTK_CONTENT_FIT_FILL);
  gtk_picture_set_alternative_text (GTK_PICTURE(picture), "测试图片内容");

  gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), picture);
  gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  gtk_window_present (GTK_WINDOW (win));

  g_timeout_add (100, timeout_cb, picture);
}

int
main (int argc, char *argv[]) {
  g_autoptr (AdwApplication) app = adw_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}