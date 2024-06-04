#include <adwaita.h>

static void 
app_activate (GApplication *app, gpointer *user_data) {

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return 0;
}