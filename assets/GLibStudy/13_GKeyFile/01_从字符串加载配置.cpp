#include <glib.h>

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

  const gchar *data =
    "[group1]\n"
    "key1=value1\n"
    "key2=value2\r\n"
    "[group2]\r\n"
    "key3=value3\r\r\n"
    "key4=value4\n";

  /* 有空格的不行，不知道为什么，我看测试里面可以啊 */
  // const gchar *data =
  //   "[group1]\n"
  //   "key1 = value1\n"
  //   "key2\t=\tvalue2\n"
  //   " [ group2 ] \n"
  //   "key3  =  value3  \n"
  //   "key4  =  value \t4\n"
  //   "  key5  =  value5\n";

  GKeyFile *keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, G_KEY_FILE_NONE, &error);
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_clear_error (&error);
  }

  gchar *value = g_key_file_get_string (keyfile, "group2", "key3", &error);
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_clear_error (&error);
  } else if (value != NULL){
    g_print ("%s\n", value);
    g_free (value);
  }

  g_key_file_set_integer (keyfile, "group2", "key3", 666);
  g_key_file_set_comment (keyfile, "group2", "key3", "这是测试数字", &error);
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_clear_error (&error);
  }

  gboolean ret = g_key_file_save_to_file (keyfile, "config.ini", &error);
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_clear_error (&error);
  } else if (ret == TRUE)
    g_print ("success save!\n");

  g_print ("keyfile->ref_count = %d\n", ((Test_GKeyFile *)keyfile)->ref_count);

  g_key_file_unref (keyfile);

  g_print ("keyfile->ref_count = %d\n", ((Test_GKeyFile *)keyfile)->ref_count);

  return 0;
}