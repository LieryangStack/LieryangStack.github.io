#include <glib-object.h>

#define T_TYPE_DOUBLE  (t_double_get_type ())
G_DECLARE_FINAL_TYPE (TDouble, t_double, T, DOUBLE, GObject)

struct _TDoubleClass {
  GObjectClass parent_class;
};

struct _TDouble {
  GObject parent;
  double value;
};

G_DEFINE_TYPE (TDouble, t_double, G_TYPE_OBJECT)

static void
t_double_class_init (TDoubleClass *class) {
  g_print ("%s\n", __func__);
}

static void
t_double_init (TDouble *self) {
  g_print ("%s\n", __func__);
}

int
main (int argc, char **argv) {
  GType dtype;
  TDouble *d, *d1;

  dtype = t_double_get_type (); /* or dtype = T_TYPE_DOUBLE */
  if (dtype)
    g_print ("Registration was a success. The type is %lx.\n", dtype);
  else
    g_print ("Registration failed.\n");

  d = g_object_new (T_TYPE_DOUBLE, NULL);
  d1 = g_object_new (T_TYPE_DOUBLE, NULL);
  if (d)
    g_print ("Instantiation was a success. The instance address is %p.\n", d);
  else
    g_print ("Instantiation failed.\n");

  if (T_IS_DOUBLE (d))
    g_print ("d is TDouble instance.\n");
  else
    g_print ("d is not TDouble instance.\n");

  if (G_IS_OBJECT (d))
    g_print ("d is GObject instance.\n");
  else
    g_print ("d is not GObject instance.\n");
  g_object_unref (d);
  g_object_unref (d);


  return 0;
}

