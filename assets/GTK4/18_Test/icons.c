#include <adwaita.h>
#include "resources.h"

static void 
app_activate (GApplication *app, gpointer *user_data) {

  GtkIconTheme *theme = gtk_icon_theme_get_for_display (gdk_display_get_default ());
  gtk_icon_theme_add_resource_path (theme, "/vpf/icons");

  gchar *name = gtk_icon_theme_get_theme_name (theme);
  // gtk_icon_theme_set_theme_name (theme, "hicolor");
  g_print ("theme name = %s\n", name);
  gboolean have_icon = gtk_icon_theme_has_icon (theme, "vpf-photo-symbolic");
  g_print ("have %d icon\n", have_icon);

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 100, 100);

  GtkWidget *image = gtk_image_new_from_icon_name ("vpf-photo-symbolic");
  // image = gtk_image_new_from_resource ("/vpf/vpf-photo-symbolic.svg");

  /* MIN {width, height} 设定为 @pixel_size */
  gtk_image_set_pixel_size (GTK_IMAGE(image), 200);

  gtk_widget_set_hexpand (image, TRUE);
  gtk_widget_set_vexpand (image, TRUE);

  gtk_window_set_child (GTK_WINDOW(win), image);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char *argv[]) {
  g_autoptr (AdwApplication) app = adw_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}