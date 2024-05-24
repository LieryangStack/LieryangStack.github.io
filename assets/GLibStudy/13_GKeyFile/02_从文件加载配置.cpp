#include <glib.h>
#include <glib-object.h>

#define CHECK_ERROR(error) \
if (error != NULL) { \
  g_critical ("%s\n", error->message); \
  g_clear_error (&error); \
}\

typedef struct _Test_GKeyFile Test_GKeyFile;

typedef struct _GKeyFileGroup GKeyFileGroup;

struct _Test_GKeyFile
{
  GList *groups;
  GHashTable *group_hash;

  GKeyFileGroup *start_group;
  GKeyFileGroup *current_group;

  GString *parse_buffer; /* Holds up to one line of not-yet-parsed data */

  gchar list_separator;

  GKeyFileFlags flags;

  gboolean checked_locales;  /* TRUE if @locales has been initialised */
  gchar **locales;  /* (nullable) */

  gint ref_count;  /* (atomic) */
};

int
main (int argc, char *argv[]) {

  GError *error = NULL;
  
  GKeyFile *keyfile = g_key_file_new ();
  g_key_file_load_from_file (keyfile, "config.ini", G_KEY_FILE_KEEP_COMMENTS, &error);
  CHECK_ERROR (error);

  gchar *value = g_key_file_get_string (keyfile, "group2", "key3", &error);
  CHECK_ERROR (error);
  if (value != NULL){
    g_print ("%s\n", value);
    g_free (value);
  }

  g_key_file_set_integer (keyfile, "group2", "key3", 666);
  g_key_file_set_comment (keyfile, "group2", "key3", "这是测试数字", &error);
  CHECK_ERROR (error);

  gboolean ret = g_key_file_save_to_file (keyfile, "config.ini", &error);
  CHECK_ERROR (error);
  if (ret == TRUE)
    g_print ("success save!\n");

  g_print ("keyfile->ref_count = %d\n", ((Test_GKeyFile *)keyfile)->ref_count);

  g_key_file_unref (keyfile);

  g_print ("keyfile->ref_count = %d\n", ((Test_GKeyFile *)keyfile)->ref_count);

  return 0;
}