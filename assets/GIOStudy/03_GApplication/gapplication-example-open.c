#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

static void
startup_cb (GApplication *app)
{
  g_print ("%s\n", __func__);
  g_application_release (app); /* 会再迭代一次看有无事件么有处理，或者是service，则有超时等待 */
  g_application_quit (app); /* 直接退出，不会再去迭代，或者超时等待 */
}

static void
activate (GApplication *application)
{
  g_print ("%s\n", __func__);
  g_print ("%s\n", g_get_prgname());
  /* 注意：在此处执行返回主循环的较长时间的操作时，
  * 应使用 g_application_hold() 和 g_application_release() 
  * 来保持应用程序在操作完成之前保持运行。
  */
  g_application_release (application);
  //  while(1);
}

static void
app_open (GApplication  *application,
          GFile        **files,
          gint           n_files,
          const gchar   *hint)
{
  g_print ("%s\n", __func__);
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
                           G_APPLICATION_IS_SERVICE | G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
  g_signal_connect (app, "startup", G_CALLBACK (startup_cb), NULL);
  g_application_set_inactivity_timeout (app, 1000);

  g_application_hold (app);
  
  status = g_application_run (app, argc, argv);

  g_object_unref (app);
  
  return status;
}
