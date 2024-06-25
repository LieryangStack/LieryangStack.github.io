#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#define G_SETTINGS_ENABLE_BACKEND
#include <gio/gsettingsbackend.h>

/* cp org.vpf.camera.gschema.xml /usr/share/glib-2.0/schemas
 * glib-compile-schemas /usr/share/glib-2.0/schemas 
 *
 * cp org.vpf.camera.gschema.xml /usr/local/gtk-4.8.3/share/glib-2.0/schemas
 * glib-compile-schemas /usr/local/gtk-4.8.3/share/glib-2.0/schemas
 */

int 
main (int argc, char *argv[]) {

  g_setenv ("GSETTINGS_SCHEMA_DIR", ".", FALSE);

  // gchar *schema_text;
  // g_file_get_contents ("./org.gtk.test.gschema.xml.orig", &schema_text, NULL, NULL);
  // g_file_set_contents ("org.gtk.test.gschema.xml", schema_text, -1, NULL);
  // g_free (schema_text);



  // gchar *keyfile_path = g_build_filename ("./", "keyfile", NULL);
  // gchar *store_path = g_build_filename (keyfile_path, "gsettings.store", NULL);

  
  GSettingsBackend *kf_backend = g_keyfile_settings_backend_new ("config.ini", "/", NULL);
  GSettings *settings = g_settings_new_with_backend ("org.gtk.exampleapp", kf_backend);
  g_object_unref (kf_backend);

  gchar* alarm_wind_popwer = g_settings_get_string (settings, "font");

  g_print ("alarm_wind_popwer = %s\n", alarm_wind_popwer);

  // g_settings_set_uint (settings, "camera-id", 111);

  // // 将更改保存到配置文件
  // // g_settings_sync();

  // // g_print ("keyfile_path = %s\n", keyfile_path);
  // // g_print ("store_path = %s\n", store_path);

  // alarm_wind_popwer = g_settings_get_uint (settings, "camera-id");

  // g_print ("alarm_wind_popwer = %d\n", alarm_wind_popwer);

  g_object_unref (settings);
  // g_free (store_path);
  // g_free (keyfile_path);

  return 0;
}