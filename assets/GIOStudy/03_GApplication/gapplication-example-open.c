#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

static void
activate (GApplication *application)
{
  g_print ("activated\n");

  /* 注意：在此处执行返回主循环的较长时间的操作时，
  * 应使用 g_application_hold() 和 g_application_release() 
  * 来保持应用程序在操作完成之前保持运行。
  */
}

static void
app_open (GApplication  *application,
          GFile        **files,
          gint           n_files,
          const gchar   *hint)
{
  gint i;

  for (i = 0; i < n_files; i++)
    {
      gchar *uri = g_file_get_uri (files[i]);
      g_print ("open %s\n", uri);
      g_free (uri);
    }

  /* 注意：在此处执行返回主循环的较长时间的操作时，
  * 应使用 g_application_hold() 和 g_application_release() 
  * 来保持应用程序在操作完成之前保持运行。
  */
}

int
main (int argc, char **argv)
{
  GApplication *app;
  int status;

  app = g_application_new ("org.gtk.TestApplication",
                           G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
  g_application_set_inactivity_timeout (app, 10000);

  status = g_application_run (app, argc, argv);

  g_object_unref (app);

  return status;
}
