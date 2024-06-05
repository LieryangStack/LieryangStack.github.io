#include <adwaita.h>

static void
selection_cb (GtkSingleSelection *sel,
              GParamSpec         *pspec,
              gpointer            user_data) {
  
  gint position = gtk_single_selection_get_selected (sel);
  g_print ("position = %d\n", position);
  // GtkListItem *item = GTK_LIST_ITEM (gtk_single_selection_get_selected_item (sel));

  // GtkStringList* list = GTK_STRING_LIST(gtk_single_selection_get_model (sel));
  // gtk_string_list_remove (list, position);
  
}

static void 
app_activate (GApplication *app, gpointer *user_data) {
  GtkBuilder *build = gtk_builder_new_from_file ("listview.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  GtkWidget *list_view = GTK_WIDGET (gtk_builder_get_object (build, "list_view"));
  GtkSingleSelection *single_selection = GTK_SINGLE_SELECTION (gtk_builder_get_object (build, "single_selection"));

  // g_signal_connect (single_selection, "notify::selected-item", G_CALLBACK (selection_cb), NULL);
  g_signal_connect (single_selection, "notify::selected", G_CALLBACK (selection_cb), NULL);


  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));
  
}

int
main (int argc, char *argv[]) {
  g_autoptr (AdwApplication) app = adw_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}