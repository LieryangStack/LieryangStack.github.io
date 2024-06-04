#include <adwaita.h>

static void 
app_activate (GApplication *app, gpointer *user_data) {
  GtkBuilder *build = gtk_builder_new_from_file ("01_headerbar.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  GtkWidget *headerbar = GTK_WIDGET (gtk_builder_get_object (build, "headerbar"));
  
  GdkDisplay *display = gdk_display_get_default ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  GFile *file = g_file_new_for_path ("css_test.css");
  gtk_css_provider_load_from_file (provider, file);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));

  g_print ("%d\n", adw_header_bar_get_show_start_title_buttons (ADW_HEADER_BAR (headerbar)));
  /* 已经设置成True，不知道为什么不显示 */
}

int
main (int argc, char *argv[]) {
  g_autoptr (AdwApplication) app = adw_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}