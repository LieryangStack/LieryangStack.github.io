#include <gtk/gtk.h>

static gboolean drop(GtkDropTarget *target,
                     const GValue  *value,
                     double         x,
                     double         y,
                     gpointer       data){
    printf("test\n");
    return TRUE;
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    GtkDropTarget* dnd = gtk_drop_target_new(GDK_TYPE_FILE_LIST,GDK_ACTION_COPY);

    g_signal_connect (dnd, "drop", G_CALLBACK (drop), NULL);
    gtk_widget_add_controller(window,GTK_EVENT_CONTROLLER(dnd));

    gtk_widget_show (window);
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}