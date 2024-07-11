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

  gtk_window_present (GTK_WINDOW (win));

  GdkDisplay *diplay = gdk_display_get_default ();

  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  GdkGLContext *gl_context = gdk_gl_context_get_current ();

  g_print ("gl_context = %p\n", gl_context);

  
  
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}