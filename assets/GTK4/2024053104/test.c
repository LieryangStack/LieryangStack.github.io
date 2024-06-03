#include <adwaita.h>

int
main (int argc, char **argv)
{
 GtkWidget *window;
  // Initialize i18n support with bindtextdomain(), etc.

  // ...

  // Initialize the widget set
  gtk_init ();

  // Create the main window
  window = gtk_window_new ();

  // Set up our GUI elements

  // ...

  // Show the application window
  gtk_window_present (GTK_WINDOW (window));

  // Enter the main event loop, and wait for user interaction
  while (1)
    g_main_context_iteration (NULL, TRUE);

  // The user lost interest
  return 0;
}
