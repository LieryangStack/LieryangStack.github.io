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


static void
describe_and_activate_action (GActionGroup *group,
                              const gchar  *name)
{
  const GVariantType *param_type;
  GVariant *state;
  gboolean enabled;
  gchar *tmp;

  param_type = g_action_group_get_action_parameter_type (group, name);
  state = g_action_group_get_action_state (group, name);
  enabled = g_action_group_get_action_enabled (group, name);

  g_print ("action name:      %s\n", name);
  tmp = param_type ? g_variant_type_dup_string (param_type) : NULL;
  g_print ("parameter type:   %s\n", tmp ? tmp : "<none>");
  g_free (tmp);
  g_print ("state type:       %s\n",
           state ? g_variant_get_type_string (state) : "<none>");
  tmp = state ? g_variant_print (state, FALSE) : NULL;
  g_print ("state:            %s\n", tmp ? tmp : "<none>");
  g_free (tmp);
  g_print ("enabled:          %s\n", enabled ? "true" : "false");

  if (state != NULL)
    g_variant_unref (state);

  /* 手动激活（触发）action */
  g_action_group_activate_action (group, name, NULL);
}

int
main (int argc, char **argv)
{
  int status;

  GApplication *app = g_application_new ("org.gtk.TestApplication", 0);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_application_set_inactivity_timeout (app, 1000);

  GSimpleAction *action;
  
  /* @name: 动作的名称，比如 open */
  action = g_simple_action_new ("simple-action", NULL);
  /* 设定激活回调函数 */
  g_signal_connect (action, "activate", G_CALLBACK (open_action), app);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (action));
  g_object_unref (action);

  /* 触发Action */
  g_action_activate (G_ACTION(action), NULL);

  /* 应用程序需要被注册，否则无法使用action */
  g_application_register (app, NULL, NULL);
  describe_and_activate_action (G_ACTION_GROUP (app), "simple-action");

  status = g_application_run (app, argc, argv);

  g_object_unref (app);

  return status;
}