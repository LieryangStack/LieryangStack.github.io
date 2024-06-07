#include <adwaita.h>

static void
item_value_changed (GtkEditableLabel   *widget,
                    GParamSpec         *pspec,
                    gpointer            user_data) {

  gint editable =  gtk_editable_label_get_editing (widget);
  
  g_print ("editable = %d\n", editable);

  // GtkListItem *item = GTK_LIST_ITEM (gtk_single_selection_get_selected_item (sel));

  // GtkStringList* list = GTK_STRING_LIST(gtk_single_selection_get_model (sel));
  // gtk_string_list_remove (list, position);
  
}

GtkWidget *popover;

static void
window_new (GSimpleAction *action,
            GVariant      *parameter,
            gpointer       user_data)
{
  g_print ("%s\n", __func__);
      gtk_popover_popup (GTK_POPOVER (popover));
}

static GActionEntry action_entries[] = {
  { "window-new", window_new },
  { "tab-new", window_new },
};

static void 
app_activate (GApplication *app, gpointer *user_data) {
  GtkBuilder *build = gtk_builder_new_from_file ("entry.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  popover = GTK_WIDGET (gtk_builder_get_object (build, "popover"));
  GtkWidget *editable_label = GTK_WIDGET (gtk_builder_get_object (build, "editable_label"));
  g_signal_connect (editable_label, "notify::editing", G_CALLBACK (item_value_changed), NULL);



  GActionMap *action_map = G_ACTION_MAP (g_simple_action_group_new ());
  g_action_map_add_action_entries (action_map,
                                   action_entries,
                                   G_N_ELEMENTS (action_entries),
                                   NULL);
  gtk_widget_insert_action_group (GTK_WIDGET (win),
                                  "win",
                                  G_ACTION_GROUP (action_map));
  // gtk_application_set_menubar (GTK_APPLICATION (app),
  //                              G_MENU_MODEL (gtk_builder_get_object (build, "menubar")));
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));

  cairo_rectangle_int_t p = {0, 0, 100, 100};

  gtk_popover_set_has_arrow (GTK_POPOVER (popover), TRUE);
  // gtk_popover_set_position (GTK_POPOVER (popover), GTK_POS_LEFT);
  gtk_popover_set_pointing_to (GTK_POPOVER (popover), &p);
  gtk_widget_set_parent (popover, editable_label);
}

int
main (int argc, char *argv[]) {
  g_autoptr (AdwApplication) app = adw_application_new ("test.application", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}