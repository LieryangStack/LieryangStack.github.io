#include "gst-app.h"

static void
handle_file_or_directory (const gchar * filename)
{
  GError *err = NULL;
  GDir *dir;
  gchar *uri;

  if ((dir = g_dir_open (filename, 0, NULL))) {
    const gchar *entry;

    while ((entry = g_dir_read_name (dir))) {
      gchar *path;

      path = g_strconcat (filename, G_DIR_SEPARATOR_S, entry, NULL);
      handle_file_or_directory (path);
      g_free (path);
    }

    g_dir_close (dir);
    return;
  }

  if (g_path_is_absolute (filename)) {
    uri = g_filename_to_uri (filename, NULL, &err);
  } else {
    gchar *curdir, *absolute_path;

    curdir = g_get_current_dir ();
    absolute_path = g_strconcat ( curdir, G_DIR_SEPARATOR_S, filename, NULL);
    uri = g_filename_to_uri (absolute_path, NULL, &err);
    g_free (absolute_path);
    g_free (curdir);
  }

  if (uri) {
    /* great, we have a proper file:// URI, let's play it! */
    play_uri (uri);
  } else {
    g_warning ("Failed to convert filename '%s' to URI: %s", filename,
        err->message);
    g_error_free (err);
  }

  g_free (uri);
}

int
main (int argc, char *argv[])
{
  gchar **filenames = NULL;
  const GOptionEntry entries[] = {
    /* you can add your won command line options here */
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames,
      "Special option that collects any remaining arguments for us" },
    { NULL, }
  };
  GOptionContext *ctx;
  GError *err = NULL;
  gint i, num;

  ctx = g_option_context_new ("[FILE1] [FILE2] ...");
  g_option_context_add_group (ctx, gst_init_get_option_group ());
  g_option_context_add_main_entries (ctx, entries, NULL);

  if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
    g_print ("Error initializing: %s\n", GST_STR_NULL (err->message));
    return -1;
  }
  g_option_context_free (ctx);

  if (filenames == NULL || *filenames == NULL) {
    g_print ("Please specify a file to play\n\n");
    return -1;
  }



  num = g_strv_length (filenames);

  for (i = 0; i < num; ++i) {
    handle_file_or_directory (filenames[i]);
  }

  g_strfreev (filenames);

  return 0;
}
