#include <gtk/gtk.h>

static void
selection_changed ( GtkSelectionModel* self, guint position, guint n_items, gpointer user_data){
  GListModel *list_mode =  gtk_single_selection_get_model (GTK_SINGLE_SELECTION (self));
  GtkStringList *s1 = GTK_STRING_LIST (list_mode);
  g_print ("position = %d  n_items = %d\n", position, n_items);
  g_print ("select %s\n", gtk_string_list_get_string (s1, position));
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = gtk_application_window_new (app);
  gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
  GtkWidget *scr = gtk_scrolled_window_new ();
  gtk_window_set_child (GTK_WINDOW (win), scr);

  char *array[] = {
    "one", "two", "three", "four", NULL
  };
  GtkStringList *sl = gtk_string_list_new ((const char * const *) array);

  /* position_0 = one， 列表从位置0开始记录 */
  // g_print ("position_0 = %s", gtk_string_list_get_string (sl, 0));

  GtkSingleSelection *ss = gtk_single_selection_new (G_LIST_MODEL (sl));

  g_signal_connect (ss, "selection-changed", (GCallback)selection_changed, NULL);

  const char *ui_string =
  "<interface>"
    "<template class=\"GtkListItem\">"
      "<property name=\"child\">"
        "<object class=\"GtkLabel\">"
          "<binding name=\"label\">"
            "<lookup name=\"string\" type=\"GtkStringObject\">"
              "<lookup name=\"item\">GtkListItem</lookup>"
            "</lookup>"
          "</binding>"
        "</object>"
      "</property>"
    "</template>"
  "</interface>";

  GBytes *gbytes = g_bytes_new_static (ui_string, strlen (ui_string));
  GtkListItemFactory *factory = gtk_builder_list_item_factory_new_from_bytes (NULL, gbytes);

  GtkWidget *lv = gtk_list_view_new (GTK_SELECTION_MODEL (ss), factory);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), lv);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
}


/* ----- main ----- */
#define APPLICATION_ID "com.github.ToshioCP.list2"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  gtk_init ();

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}


