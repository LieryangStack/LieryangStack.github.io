#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

static void
activate (GApplication *application)
{
  g_application_hold (application);
  g_print ("activated\n");
  g_application_release (application);
}


/**
 * @brief: 动作触发回调函数
*/
static void
open_action_cb (GAction  *action,
                GVariant *parameter,
                gpointer  data) {
  
  GApplication *application = G_APPLICATION (data);

  g_application_hold (application);
  g_print ("open_action() %s activated\n", g_action_get_name (action));
  g_application_release (application);
}

int
main (int argc, char **argv)
{
  int status;

  GApplication *app = g_application_new ("org.gtk.TestApplication", 0);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_application_set_inactivity_timeout (app, 1000);


  GSimpleActionGroup *group = g_simple_action_group_new ();
  GSimpleAction *simple = g_simple_action_new ("open", NULL);
  g_signal_connect (simple, "activate", G_CALLBACK (open_action_cb), app);

  
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (simple));
  g_object_unref (simple);

  g_action_group_activate_action (G_ACTION_GROUP (group), "open", NULL);

  /**
   * 1. 你也可以把该 GSimpleActionGroup 替换 GApplication初始的 GSimpleActionGroup
   * 2. 或者把这个Action添加到 GApplication初始的 GSimpleActionGroup 中
  */
   
  /* 方式一： */
  // g_application_set_action_group (app, G_ACTION_GROUP (group));
  // g_object_unref (group);

  /* 方式二： */
  /* 应用程序需要被注册，否则无法使用应用程序默认的action组 */
  g_application_register (app, NULL, NULL);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (simple));
  g_action_group_activate_action (G_ACTION_GROUP (app), "open", NULL);

  status = g_application_run (app, argc, argv);

  g_object_unref (app);
  g_object_unref (group);

  return status;
}