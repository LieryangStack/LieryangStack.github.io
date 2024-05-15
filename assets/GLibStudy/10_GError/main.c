#include <glib.h>

#define G_IO_VPF g_vpf_error_quark ()

static GQuark 
g_vpf_error_quark (void) {
  return g_quark_from_static_string ("g-vpf-error-quark");
}

typedef enum
{
  G_VPF_ERROR_NOTOPEN,
} GVpfError;


int
main (int argc, char *argv[]) {

  GError *error = NULL; 
  
  error = g_error_new_literal (G_FILE_ERROR, G_FILE_ERROR_NOENT, "No such file or directory");

  if (error != NULL) {
    g_print ("G_FILE_ERROR = %s\n", g_quark_to_string (G_FILE_ERROR));
    g_critical ("%s:%s", __func__, error->message);
    g_error_free (error); /* GError没有引用计数，通过 g_error_free 就能完全释放内存*/
  }
  
  error = g_error_new (G_IO_VPF, G_VPF_ERROR_NOTOPEN,
                       "GVpf.Error:%s: %s",
                       __func__,
                       "Not open the VPF");
  if (error != NULL) {
    g_critical ("%s\n", error->message);
    g_clear_error (&error);
  }

  return 0;
}